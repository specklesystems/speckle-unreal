// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/StaticMeshConverter.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "SpeckleUnrealManager.h"
#include "StaticMeshOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Objects/Mesh.h"
#include "Objects/RenderMaterial.h"
#include "LogSpeckle.h"


UStaticMeshConverter::UStaticMeshConverter()
{
	SpeckleTypes.Add(UMesh::StaticClass());
	
	Transient = false;
	UseFullBuild = true;
	BuildSimpleCollision = true;
	
	MeshActorType = AStaticMeshActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}

AActor* UStaticMeshConverter::CreateEmptyActor(const ASpeckleUnrealManager* Manager, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AActor* Actor = Manager->GetWorld()->SpawnActor<AActor>(MeshActorType, Transform, SpawnParameters);
	Actor->GetRootComponent()->SetMobility(ActorMobility);
	return Actor;
}

AActor* UStaticMeshConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager)
{
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Geometry"), SpeckleBase->Id);
	UPackage* Package = CreatePackage(*PackagePath);

	const UMesh* SpeckleMesh = Cast<UMesh>(SpeckleBase);
	if(SpeckleMesh == nullptr) return nullptr;
	
	//Find existing mesh
	UStaticMesh* Mesh = Cast<UStaticMesh>(Package->FindAssetInPackage());
	
	if(!IsValid(Mesh))
	{
		//No existing mesh was found, try and convert SpeckleMesh
		Mesh = MeshToNative(Package, SpeckleMesh, Manager);
	}
	
	AActor* Actor = CreateEmptyActor(Manager, FTransform(SpeckleMesh->Transform));
	TInlineComponentArray<UStaticMeshComponent*> Components;
	Actor->GetComponents<UStaticMeshComponent>(Components);
	
	UStaticMeshComponent* MeshComponent;
	if(Components.Num() > 0) MeshComponent = Components[0];
	else
	{
		// MeshActorType doesn't have a UStaticMeshComponent, so we will add one
		MeshComponent = NewObject<UStaticMeshComponent>(Actor, FName("SpeckleMeshComponent"));
		MeshComponent->SetupAttachment(Actor->GetRootComponent());
		MeshComponent->RegisterComponent();
	}
	
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetMaterial(0, GetMaterial(SpeckleMesh->RenderMaterial, Manager));
	
	return Actor;
}



UStaticMesh* UStaticMeshConverter::MeshToNative(UObject* Outer,  const UMesh* SpeckleMesh,
													ASpeckleUnrealManager* Manager)
{
	TArray<UMesh*> Meshes;
	Meshes.Add(const_cast<UMesh*>(SpeckleMesh));
	return MeshesToNative(Outer, SpeckleMesh, Meshes, Manager);
}


UStaticMesh* UStaticMeshConverter::MeshesToNative(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes,
                                                    ASpeckleUnrealManager* Manager)
{
	if(SpeckleMeshes.Num() == 0) return nullptr;
	
	const EObjectFlags ObjectFags = Transient? RF_Transient | RF_Public : RF_Public;
	UStaticMesh* Mesh = NewObject<UStaticMesh>(Outer, FName(Parent->Id), ObjectFags);

	Mesh->InitResources();
	Mesh->SetLightingGuid();

	UStaticMeshDescription* StaticMeshDescription = Mesh->CreateStaticMeshDescription(Outer);
	FMeshDescription& BaseMeshDescription = StaticMeshDescription->GetMeshDescription();

	//Build Settings
#if WITH_EDITOR
	{
		FStaticMeshSourceModel& SrcModel = Mesh->AddSourceModel();
		SrcModel.BuildSettings.bRecomputeNormals = true;
		SrcModel.BuildSettings.bRecomputeTangents = true;
		SrcModel.BuildSettings.bRemoveDegenerates = false;
		SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
		SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
		SrcModel.BuildSettings.bGenerateLightmapUVs = GenerateLightmapUV;
		SrcModel.BuildSettings.SrcLightmapIndex = 0;
		SrcModel.BuildSettings.DstLightmapIndex = 1;
	}
#endif

	UStaticMesh::FBuildMeshDescriptionsParams MeshParams;
	GenerateMeshParams(MeshParams);

	for(const UMesh* SpeckleMesh : SpeckleMeshes)
	{
		const size_t NumberOfVertices = SpeckleMesh->Vertices.Num();
		const size_t NumberOfFaces = SpeckleMesh->Faces.Num();
		
		// Convert Vertices
		if(NumberOfVertices == 0 || NumberOfFaces == 0) continue;
		
		StaticMeshDescription->ReserveNewVertices(NumberOfVertices);
		
		TArray<FVertexID> Vertices;
		Vertices.Reserve(NumberOfVertices);

		for(const FVector VertexPosition : SpeckleMesh->Vertices)
		{
			const FVertexID VertID = StaticMeshDescription->CreateVertex();
			StaticMeshDescription->SetVertexPosition(VertID, VertexPosition);
			Vertices.Add(VertID);
		}
		
		// Convert Material
		UMaterialInterface* Material = GetMaterial(SpeckleMesh->RenderMaterial, Manager);
	
		const FName MaterialSlotName = Mesh->AddMaterial(Material);;
		BaseMeshDescription.PolygonGroupAttributes().RegisterAttribute<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName, 1, MaterialSlotName,  EMeshAttributeFlags::None);

		// Convert Faces
		const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();

		StaticMeshDescription->SetPolygonGroupMaterialSlotName(PolygonGroupID, MaterialSlotName);
	
		StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 2, FVector2D::ZeroVector, EMeshAttributeFlags::None);

		StaticMeshDescription->ReserveNewTriangles(SpeckleMesh->Faces.Num() * 3); //Reserve space assuming faces will all be triangles
		StaticMeshDescription->ReserveNewPolygons(SpeckleMesh->Faces.Num());
		StaticMeshDescription->ReserveNewVertexInstances(SpeckleMesh->Faces.Num() * 3); //Reserve space assuming faces will all be triangles

		int32 i = 0;
		while (i < NumberOfFaces)
		{
			int32 n = SpeckleMesh->Faces[i];
			if(n < 3) n += 3; // 0 -> 3, 1 -> 4
		
			TArray<FVertexInstanceID> VertexInstances;
			VertexInstances.Reserve(n);
			TSet<FVertexID> Verts;
			Verts.Reserve(n);
			for(int j = 0; j < n; j ++)
			{
				int32 VertIndex = SpeckleMesh->Faces[i + n - j];
				FVertexID Vert = Vertices[VertIndex];
				bool AlreadyInSet;
				Verts.Add(Vert, &AlreadyInSet);
	
				if(AlreadyInSet)
				{
					UE_LOG(LogSpeckle, Warning, TEXT("Invalid Polygon while creating mesh %s - vertex at index %d appears more than once in a face, duplicate vertices will be ignored"), *SpeckleMesh->Id, VertIndex);
					continue;
				}
				FVertexInstanceID VertexInstance = StaticMeshDescription->CreateVertexInstance(Vert);
				
				VertexInstances.Add(VertexInstance);
			
				if(SpeckleMesh->TextureCoordinates.Num() > VertIndex)
					StaticMeshDescription->SetVertexInstanceUV(VertexInstance, SpeckleMesh->TextureCoordinates[VertIndex]);
			
				//if(SpeckleMesh->VertexColors.Num() > VertIndex)
				//	//TODO set vertex colors
			}
			
			i += n + 1;
			if(VertexInstances.Num() < 3)
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Polygon while creating mesh %s - face has fewer than 3 verts, this face will be ignored"), *SpeckleMesh->Id);
				continue;
			}
		
		
			TArray<FEdgeID> Edges;
			Edges.Reserve(n);
	
			const FPolygonID PolygonID = StaticMeshDescription->CreatePolygon(PolygonGroupID, VertexInstances, Edges);

			for (const FEdgeID EdgeID : Edges)
			{
				StaticMeshDescription->GetEdgeHardnesses()[EdgeID] = true;
			}
			
			StaticMeshDescription->ComputePolygonTriangulation(PolygonID);
		}
	}

	if(StaticMeshDescription->Vertices().Num() == 0
		|| StaticMeshDescription->VertexInstances().Num() == 0
		|| StaticMeshDescription->Triangles().Num() == 0)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Skipping %s $s, converted mesh is empty!"), *Parent->SpeckleType, *Parent->Id);
		return nullptr;
	}
			
	
#if ENGINE_MAJOR_VERSION <= 4
	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Binormal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Center, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	FStaticMeshOperations::ComputePolygonTangentsAndNormals(BaseMeshDescription);
#else
	FStaticMeshOperations::ComputeTriangleTangentsAndNormals(BaseMeshDescription);
#endif
	
	//FStaticMeshOperations::ComputeTangentsAndNormals(BaseMeshDescription, EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents);
	
	
	//Mesh->PreEditChange(nullptr);
	
#if ENGINE_MAJOR_VERSION <= 4
	Mesh->LightMapCoordinateIndex = 1;
#else
	Mesh->SetLightMapCoordinateIndex(1);
#endif
	
	Mesh->BuildFromMeshDescriptions(TArray<const FMeshDescription*>{&BaseMeshDescription}, MeshParams);

#if WITH_EDITOR
	if(UseFullBuild) Mesh->Build(true); //This makes conversion time much slower, but is needed for generating lightmap UVs

	if (GIsEditor && !GWorld->HasBegunPlay())
	{
		Mesh->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Mesh);
	}
#endif
	//Mesh->PostEditChange(); //This doesn't seem to be required

	return Mesh;
}

void UStaticMeshConverter::GenerateMeshParams(UStaticMesh::FBuildMeshDescriptionsParams& MeshParams) const
{
	MeshParams.bBuildSimpleCollision = BuildSimpleCollision;
	MeshParams.bCommitMeshDescription = true;
	MeshParams.bMarkPackageDirty = true;
	MeshParams.bUseHashAsGuid = false;
}

UMaterialInterface* UStaticMeshConverter::GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager)
{
	if(SpeckleMaterial == nullptr || SpeckleMaterial->Id == "") return Manager->DefaultMeshMaterial; //Material is invalid
	
	UMaterialInterface* ExistingMaterial;
	if(Manager->TryGetMaterial(SpeckleMaterial, true, ExistingMaterial))
		return ExistingMaterial; //Return existing material

	
	UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
	    ? Manager->BaseMeshOpaqueMaterial
	    : Manager->BaseMeshTransparentMaterial;
	
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Materials"), SpeckleMaterial->Id);
	UPackage* Package = CreatePackage(*PackagePath);

	
	UMaterialInstance* MaterialInstance;
#if WITH_EDITOR
	if (GIsEditor && !GWorld->HasBegunPlay())
	{
		const FName Name = MakeUniqueObjectName(Package, UMaterialInstanceConstant::StaticClass(), FName(SpeckleMaterial->Name));

		//TStrongObjectPtr< UMaterialInstanceConstantFactoryNew > MaterialFact( NewObject< UMaterialInstanceConstantFactoryNew >() );
		//MaterialFact->InitialParent = MaterialBase;
		//UMaterialInstanceConstant* ConstMaterial = Cast< UMaterialInstanceConstant >( MaterialFact->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, Name, RF_Public, nullptr, GWarn ) );
		UMaterialInstanceConstant* ConstMaterial = NewObject<UMaterialInstanceConstant>(Package, Name, RF_Public);
		
		MaterialInstance = ConstMaterial;
		ConstMaterial->SetParentEditorOnly(MaterialBase);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Opacity"), SpeckleMaterial->Opacity);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Metallic"), SpeckleMaterial->Metalness);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Roughness"), SpeckleMaterial->Roughness);
		ConstMaterial->SetVectorParameterValueEditorOnly(FMaterialParameterInfo("BaseColor"), SpeckleMaterial->Diffuse);
		ConstMaterial->SetVectorParameterValueEditorOnly(FMaterialParameterInfo("EmissiveColor"), SpeckleMaterial->Emissive);
		
		//ConstMaterial->InitStaticPermutation();
		
		ConstMaterial->MarkPackageDirty();

		FAssetRegistryModule::AssetCreated(MaterialInstance);
	}
	else
#endif
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialBase, Package, FName(SpeckleMaterial->Name));
		MaterialInstance = DynMaterial;
		
		DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
		DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
		DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
		DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
		DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
		
		DynMaterial->SetFlags(RF_Public);
	}
	
	Manager->ConvertedMaterials.Add(SpeckleMaterial->Id, MaterialInstance);
	
	return MaterialInstance;
	
}




UBase* UStaticMeshConverter::ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager)
{
	const UStaticMeshComponent* M = Cast<UStaticMeshComponent>(Object);

	if(M == nullptr)
	{
		const AActor* A = Cast<AActor>(Object);
		if(A != nullptr)
		{
			M = A->FindComponentByClass<UStaticMeshComponent>();
		}
	}
	if(M == nullptr) return nullptr;
	
	return MeshToSpeckle(M, Manager);
}


UMesh* UStaticMeshConverter::MeshToSpeckle(const UStaticMeshComponent* Object, ASpeckleUnrealManager* Manager)
{
	return nullptr; //TODO implement ToSpeckle function
}
