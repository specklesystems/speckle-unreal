// Fill out your copyright notice in the Description page of Project Settings.

#include "NativeActors/SpeckleUnrealStaticMesh.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "SpeckleUnrealManager.h"
#include "StaticMeshOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Objects/Mesh.h"
#include "Objects/RenderMaterial.h"


ASpeckleUnrealStaticMesh::ASpeckleUnrealStaticMesh() : ASpeckleUnrealActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("SpeckleMeshComponent"), RF_Public);
	MeshComponent->SetMobility(EComponentMobility::Stationary);
	MeshComponent->SetupAttachment(RootComponent);

	Transient = false;
	UseFullBuild = true;
	BuildSimpleCollision = true;
}

void ASpeckleUnrealStaticMesh::SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager)
{

	//Create Mesh Asset
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Geometry"),SpeckleMesh->Id);
	UPackage* Package = CreatePackage(*PackagePath);
	
	const EObjectFlags ObjectFags = Transient? RF_Transient | RF_Public : RF_Public;
	
	UStaticMesh* Mesh = NewObject<UStaticMesh>(Package, FName(SpeckleMesh->Id), ObjectFags);

	Mesh->InitResources();
	Mesh->SetLightingGuid();

	UStaticMeshDescription* StaticMeshDescription = Mesh->CreateStaticMeshDescription(RootComponent);
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
		SrcModel.BuildSettings.bGenerateLightmapUVs = true;
		SrcModel.BuildSettings.SrcLightmapIndex = 0;
		SrcModel.BuildSettings.DstLightmapIndex = 1;
	}
#endif

	UStaticMesh::FBuildMeshDescriptionsParams MeshParams;
	MeshParams.bBuildSimpleCollision = BuildSimpleCollision;
	MeshParams.bCommitMeshDescription = true;
	MeshParams.bMarkPackageDirty = true;
	MeshParams.bUseHashAsGuid = false;
	
	 //Set Mesh Data
    UMaterialInterface* Material = GetMaterial(SpeckleMesh->RenderMaterial, Manager);
	
	const FName MaterialSlotName = Mesh->AddMaterial(Material);;
	BaseMeshDescription.PolygonGroupAttributes().RegisterAttribute<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName, 1, MaterialSlotName,  EMeshAttributeFlags::None);
	{
		const size_t NumberOfVertices = SpeckleMesh->Vertices.Num();
		StaticMeshDescription->ReserveNewVertices(NumberOfVertices);
	
		TArray<FVertexID> Vertices;
		Vertices.Reserve(NumberOfVertices);
	
		for(const FVector VertexPosition : SpeckleMesh->Vertices)
		{
			const FVertexID VertID = StaticMeshDescription->CreateVertex();
			StaticMeshDescription->SetVertexPosition(VertID, VertexPosition);
			Vertices.Add(VertID);
		}
	
		//Convert Faces
		const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();

		StaticMeshDescription->SetPolygonGroupMaterialSlotName(PolygonGroupID, MaterialSlotName);
		
		StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 2, FVector2D::ZeroVector, EMeshAttributeFlags::None);


		StaticMeshDescription->ReserveNewTriangles(SpeckleMesh->Faces.Num() * 3); //Reserve space assuming faces will all be triangles
		StaticMeshDescription->ReserveNewPolygons(SpeckleMesh->Faces.Num());
		StaticMeshDescription->ReserveNewVertexInstances(SpeckleMesh->Faces.Num() * 3); //Reserve space assuming faces will all be triangles

		int32 i = 0;
		while (i < SpeckleMesh->Faces.Num())
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
					UE_LOG(LogTemp, Warning, TEXT("Invalid Polygon while creating mesh %s - vertex at index %d appears more than once in a face, duplicate vertices will be ignored"), *SpeckleMesh->Id, VertIndex);
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
    }
	
	//Mesh->PreEditChange(nullptr);
	
#if ENGINE_MAJOR_VERSION <= 4
	Mesh->LightMapCoordinateIndex = 1;
#else
	Mesh->SetLightMapCoordinateIndex(1);
#endif
	
	Mesh->BuildFromMeshDescriptions(TArray<const FMeshDescription*>{&BaseMeshDescription}, MeshParams);

#if WITH_EDITOR
	if(UseFullBuild) Mesh->Build(true); //This makes conversion time much slower, but is needed for generating lightmap UVs
#endif

	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		FAssetRegistryModule::AssetCreated(Mesh);
	}
	//Mesh->PostEditChange(); //This doesn't seem to be required
	
	MeshComponent->SetStaticMesh(Mesh);
	
	MeshComponent->SetMaterialByName(MaterialSlotName, Material);
}


UMaterialInterface* ASpeckleUnrealStaticMesh::GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager)
{
	if(SpeckleMaterial->Id == "") return Manager->DefaultMeshMaterial; //Material is invalid
	
	UMaterialInterface* ExistingMaterial;
	if(Manager->TryGetMaterial(SpeckleMaterial, true, ExistingMaterial))
		return ExistingMaterial; //Return existing material

	
	UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
	    ? Manager->BaseMeshOpaqueMaterial
	    : Manager->BaseMeshTransparentMaterial;
	
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Materials"), SpeckleMaterial->Id);
	UPackage* Package = CreatePackage(*PackagePath);
	
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialBase, Package, FName(SpeckleMaterial->Name));

	DynMaterial->SetFlags(RF_Public);
	
	DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
	DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
	DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
	DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
	DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
	
	Manager->ConvertedMaterials.Add(SpeckleMaterial->Id, DynMaterial);

	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		FAssetRegistryModule::AssetCreated(DynMaterial);
	}
	
	return DynMaterial;
	
}
