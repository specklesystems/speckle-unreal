// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/StaticMeshConverter.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "StaticMeshOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMeshActor.h"
#include "Objects/Geometry/Mesh.h"
#include "LogSpeckle.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/Converters/MaterialConverter.h"
#include "Misc/ScopedSlowTask.h"
#include "Objects/DisplayValueElement.h"
#include "Objects/Other/RenderMaterial.h"
#include "Objects/Utils/SpeckleObjectUtils.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"

UStaticMeshConverter::UStaticMeshConverter()
{
	SpeckleTypes.Add(UMesh::StaticClass());
	SpeckleTypes.Add(UDisplayValueElement::StaticClass());
	
#if WITH_EDITORONLY_DATA
	UseFullBuild = true;
	DisplayBuildProgressBar = true;
	AllowCancelBuild = false;
#endif
	Transient = false;
	BuildSimpleCollision = true;
	
	BuildReversedIndexBuffer = true;
	UseFullPrecisionUVs = false;
	RemoveDegeneratesOnBuild = true;
	MinLightmapResolution = 64;
		
	MeshActorType = AStaticMeshActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}

AActor* UStaticMeshConverter::CreateEmptyActor(UWorld* World, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AActor* Actor = World->SpawnActor<AActor>(MeshActorType, Transform, SpawnParameters);
	if(Actor->HasValidRootComponent())
		Actor->GetRootComponent()->SetMobility(EComponentMobility::Movable); //Create actor as movable for now, we will change it later to the desired mobility
	return Actor;
}

UObject* UStaticMeshConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>& AvailableConverters)
{
	const UMesh* m = Cast<UMesh>(SpeckleBase);
	if(m != nullptr)
	{
		//Handle Single Mesh
		return MeshToNativeActor(m, World, AvailableConverters);
	}
	
	const UDisplayValueElement* d = Cast<UDisplayValueElement>(SpeckleBase);
	if(d != nullptr)
	{
		//Handle Element with Display Values
		return MeshesToNativeActor(d, d->DisplayValue, World, AvailableConverters);
	}
	
	return nullptr;
}


AActor* UStaticMeshConverter::MeshToNativeActor(const UMesh* SpeckleMesh, UWorld* World, TScriptInterface<ISpeckleConverter>& MaterialConverter)
{
	TArray<UMesh*> Meshes = {const_cast<UMesh*>(SpeckleMesh)};
	return MeshesToNativeActor(SpeckleMesh, Meshes, World, MaterialConverter);
}

AActor* UStaticMeshConverter::MeshesToNativeActor(const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, UWorld* World, TScriptInterface<ISpeckleConverter>& RenderMaterialConverter)
{
	check(RenderMaterialConverter.GetInterface() != nullptr);
	ensureMsgf(Execute_CanConvertToNative(RenderMaterialConverter.GetObject(), URenderMaterial::StaticClass()), TEXT("StaticMeshConverter expects a valid RenderMaterial converter to be avaiable"));

	
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle/Geometry"), Parent->Id);
	UPackage* Package = CreatePackage(*PackagePath);
	
	//Find existing mesh
	UStaticMesh* Mesh = Cast<UStaticMesh>(Package->FindAssetInPackage());
	
	if(!IsValid(Mesh))
	{
		//No existing mesh was found, try and convert SpeckleMesh
		Mesh = MeshesToNativeMesh(Package, Parent, SpeckleMeshes, RenderMaterialConverter);
	}

	FMatrix Transform = FMatrix::Identity;
	// For single mesh, we check for transform
	//TODO figure out how to handle DisplayValueElement with transform. Maybe we just grab transform from parent unconditionally? How does this affect blocks?
	if(Parent == SpeckleMeshes[0])
	{
		Transform = SpeckleMeshes[0]->Transform;
	}
	
	AActor* Actor = CreateEmptyActor(World, USpeckleObjectUtils::CreateTransform(Transform));
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
	
	int i = 0;
	for(const UMesh* DisplayMesh : SpeckleMeshes)
	{
		URenderMaterial* MaterialToConvert = DisplayMesh->RenderMaterial;
		if(!MaterialToConvert)
		{
		    //Try and grab a material from the parent
		 	const auto* MaterialProperty = Parent->DynamicProperties.Find("renderMaterial");
			const TSharedPtr<FJsonObject>* MaterialObject;
		 	if(MaterialProperty && (*MaterialProperty)->TryGetObject(MaterialObject))
		 	{
		 		//TODO giving a nullptr transport is pretty unsafe and relies on the deserializer not to have to dereference a detached property.
		 	    MaterialToConvert = Cast<URenderMaterial>(USpeckleSerializer::DeserializeBase(*MaterialObject, nullptr));
		    }
		}
		UMaterialInterface* Material = GetMaterial(MaterialToConvert, World, RenderMaterialConverter);
		
		ensure(IsValid(Material));
		MeshComponent->SetMaterial(i, Material);

		i++;
	}

	if(Actor->HasValidRootComponent())
		Actor->GetRootComponent()->SetMobility(ActorMobility);
	
	return Actor;
}


UMaterialInterface* UStaticMeshConverter::GetMaterial(const URenderMaterial* SpeckleMaterial, UWorld* World, TScriptInterface<ISpeckleConverter>& MaterialConverter) const
{
	if(!SpeckleMaterial)
	{
		// Create a fake render material,  (since nullptr doesn't have a type, speckle converters don't know how to convert it)
		// If the converter wants to handle this specially, The material has an empty Id
		SpeckleMaterial = NewObject<URenderMaterial>(GetTransientPackage(), "NullSpeckleMaterial");
	}
	return Cast<UMaterialInterface>(Execute_ConvertToNative(MaterialConverter.GetObject(), SpeckleMaterial, World, MaterialConverter));
}


UStaticMesh* UStaticMeshConverter::MeshToNativeMesh(UObject* Outer,  const UMesh* SpeckleMesh, TScriptInterface<ISpeckleConverter>& MaterialConverter)
{
	TArray<UMesh*> Meshes;
	Meshes.Add(const_cast<UMesh*>(SpeckleMesh));
	return MeshesToNativeMesh(Outer, SpeckleMesh, Meshes, MaterialConverter);
}


UStaticMesh* UStaticMeshConverter::MeshesToNativeMesh(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, TScriptInterface<ISpeckleConverter>& MaterialConverter)
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
		SrcModel.BuildSettings.bRemoveDegenerates = RemoveDegeneratesOnBuild;
		SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
		SrcModel.BuildSettings.bBuildReversedIndexBuffer = BuildReversedIndexBuffer;
		SrcModel.BuildSettings.bUseFullPrecisionUVs = UseFullPrecisionUVs;
		SrcModel.BuildSettings.bGenerateLightmapUVs = GenerateLightmapUV;
		SrcModel.BuildSettings.SrcLightmapIndex = 0;
		SrcModel.BuildSettings.DstLightmapIndex = 1;
		SrcModel.BuildSettings.MinLightmapResolution = MinLightmapResolution;
	}
#endif

	UStaticMesh::FBuildMeshDescriptionsParams MeshParams;
	GenerateMeshParams(MeshParams);

	for(const UMesh* SpeckleMesh : SpeckleMeshes)
	{
		const size_t NumberOfVertices = SpeckleMesh->Vertices.Num();
		const size_t NumberOfFacesIndices = SpeckleMesh->Faces.Num();
		
		// Convert Vertices
		if(NumberOfVertices == 0 || NumberOfFacesIndices == 0) continue;
		
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
		UMaterialInterface* Material = GetMaterial(SpeckleMesh->RenderMaterial, GetWorld(), MaterialConverter);
	
		const FName MaterialSlotName = Mesh->AddMaterial(Material);;
		BaseMeshDescription.PolygonGroupAttributes().RegisterAttribute<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName, 1, MaterialSlotName,  EMeshAttributeFlags::None);

		// Convert Faces
		const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();

		StaticMeshDescription->SetPolygonGroupMaterialSlotName(PolygonGroupID, MaterialSlotName);
	
		StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 2, FVector2D::ZeroVector, EMeshAttributeFlags::None);

		{
			// Reserve space assuming faces will all be triangles //TODO (maybe it's better to assume something higher?)
			const int32 EstimatedNumberOfFaces = SpeckleMesh->Faces.Num() / 4 * 3;
			StaticMeshDescription->ReserveNewTriangles(EstimatedNumberOfFaces); 
			StaticMeshDescription->ReserveNewPolygons(EstimatedNumberOfFaces);
			StaticMeshDescription->ReserveNewVertexInstances(FGenericPlatformMath::Max(EstimatedNumberOfFaces * 3, SpeckleMesh->Vertices.Num()));
		}

		int32 i = 0;
		while (i < NumberOfFacesIndices)
		{
			int32 n = SpeckleMesh->Faces[i];
			if(n < 3) n += 3; // 0 -> 3, 1 -> 4
			
			TArray<FVertexInstanceID> VertexInstances;
			VertexInstances.Reserve(n);
			TSet<FVertexID> Verts;
			Verts.Reserve(n);
			for(int j = 0; j < n; j ++)
			{
				int32 VertIndex = SpeckleMesh->Faces[i + 1 + j];
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
	if(UseFullBuild)
	{
		FScopeLock Lock(&Lock_StaticMeshesToBuild);
		StaticMeshesToBuild.Add(Mesh);
	}

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

#if !WITH_EDITOR && ENGINE_MAJOR_VERSION >= 5
	MeshParams.bFastBuild = true;
#endif
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


UBase* UStaticMeshConverter::MeshToSpeckle(const UStaticMeshComponent* Object)
{
	return nullptr; //TODO implement ToSpeckle function
}


void UStaticMeshConverter::FinishConversion_Implementation()
{

	FScopeLock Lock(&Lock_StaticMeshesToBuild);

#if WITH_EDITOR
	
	FFormatNamedArguments Args;
	Args.Add( TEXT("Path"), FText::FromString( GetPathName() ) );
	const FText StatusUpdate = FText::Format( LOCTEXT("BeginStaticMeshBuildingTask", "({Path}) Building"), Args );
	FScopedSlowTask Progress(StaticMeshesToBuild.Num(), StatusUpdate, DisplayBuildProgressBar);

	Progress.MakeDialog(AllowCancelBuild);
	auto ProgressAction = [&Progress](UStaticMesh*) -> bool
	{
		Progress.EnterProgressFrame(1);
		return !Progress.ShouldCancel();
	};
	
	UStaticMesh::BatchBuild(StaticMeshesToBuild, !DisplayBuildProgressBar,  ProgressAction);
#endif

	StaticMeshesToBuild.Empty();
}

#undef LOCTEXT_NAMESPACE