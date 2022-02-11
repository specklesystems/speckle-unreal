// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/StaticMeshConverter.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "SpeckleUnrealManager.h"
#include "StaticMeshOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMeshActor.h"
#include "Objects/Mesh.h"
#include "LogSpeckle.h"
#include "Conversion/Converters/RenderMaterialConverter.h"


UStaticMeshConverter::UStaticMeshConverter()
{
	SpeckleTypes.Add(UMesh::StaticClass());
	
	Transient = false;
	UseFullBuild = true;
	BuildSimpleCollision = true;

	MaterialConverter = CreateDefaultSubobject<URenderMaterialConverter>(TEXT("Material Converter"));
	
	MeshActorType = AStaticMeshActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}

AActor* UStaticMeshConverter::CreateEmptyActor(UWorld* World, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AActor* Actor = World->SpawnActor<AActor>(MeshActorType, Transform, SpawnParameters);
	return Actor;
}


AActor* UStaticMeshConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World)
{
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle/Geometry"), SpeckleBase->Id);
	UPackage* Package = CreatePackage(*PackagePath);

	const UMesh* SpeckleMesh = Cast<UMesh>(SpeckleBase);
	if(SpeckleMesh == nullptr) return nullptr;
	
	//Find existing mesh
	UStaticMesh* Mesh = Cast<UStaticMesh>(Package->FindAssetInPackage());
	
	if(!IsValid(Mesh))
	{
		//No existing mesh was found, try and convert SpeckleMesh
		Mesh = MeshToNative(Package, SpeckleMesh);
	}
	
	AActor* Actor = CreateEmptyActor(World, FTransform(SpeckleMesh->Transform));
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
	MeshComponent->SetMaterial(0, MaterialConverter->GetMaterial(SpeckleMesh->RenderMaterial, true, !FApp::IsGame()));

	if(Actor->HasValidRootComponent())
		Actor->GetRootComponent()->SetMobility(ActorMobility);
	
	return Actor;
}



UStaticMesh* UStaticMeshConverter::MeshToNative(UObject* Outer,  const UMesh* SpeckleMesh)
{
	TArray<UMesh*> Meshes;
	Meshes.Add(const_cast<UMesh*>(SpeckleMesh));
	return MeshesToNative(Outer, SpeckleMesh, Meshes);
}


UStaticMesh* UStaticMeshConverter::MeshesToNative(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes)
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
		SrcModel.BuildSettings.bRecomputeNormals = false;
		SrcModel.BuildSettings.bRecomputeTangents = false;
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
		UMaterialInterface* Material = MaterialConverter->GetMaterial(SpeckleMesh->RenderMaterial);
	
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
	
	FStaticMeshOperations::ComputeTangentsAndNormals(BaseMeshDescription, EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents);
	
	
	//Mesh->PreEditChange(nullptr);
	
#if ENGINE_MAJOR_VERSION <= 4
	Mesh->LightMapCoordinateIndex = 1;
#else
	Mesh->SetLightMapCoordinateIndex(1);
#endif
	
	Mesh->BuildFromMeshDescriptions(TArray<const FMeshDescription*>{&BaseMeshDescription}, MeshParams);

#if WITH_EDITOR
	if(UseFullBuild) Mesh->Build(true); //This makes conversion time much slower, but is needed for generating lightmap UVs

	if (!FApp::IsGame())
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


void UStaticMeshConverter::CleanUp_Implementation()
{
	MaterialConverter->CleanUp();
}


UBase* UStaticMeshConverter::ConvertToSpeckle_Implementation(const UObject* Object)
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
	
	return MeshToSpeckle(M);
}


UMesh* UStaticMeshConverter::MeshToSpeckle(const UStaticMeshComponent* Object)
{
	return nullptr; //TODO implement ToSpeckle function
}
