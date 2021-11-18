// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeckleUnrealStaticMesh.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "SpeckleUnrealManager.h"
#include "StaticMeshOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Objects/RenderMaterial.h"


UMaterialInterface* ASpeckleUnrealStaticMesh::CreateMaterial(ASpeckleUnrealManager* Manager, const URenderMaterial& SpeckleMaterial)
{
	UMaterialInterface* ExplicitMaterial;
	if(SpeckleMaterial.Opacity >= 1)
		ExplicitMaterial = Manager->BaseMeshOpaqueMaterial;
	else
		ExplicitMaterial = Manager->BaseMeshTransparentMaterial;
		
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(ExplicitMaterial, Manager, FName(SpeckleMaterial.Name));
	
	DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial.Opacity);
	DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial.Metalness);
	DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial.Roughness);
	DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial.Diffuse);
	DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial.Emissive);
	
	Manager->ConvertedMaterials.Add(SpeckleMaterial.Id, DynMaterial);
	
	return DynMaterial;
	
}

// Sets default values
ASpeckleUnrealStaticMesh::ASpeckleUnrealStaticMesh() : ASpeckleUnrealActor()
{
    MeshComponent = NewObject<UStaticMeshComponent>(RootComponent, FName("SpeckleMeshComponent"), RF_Public);
    MeshComponent->SetMobility(EComponentMobility::Stationary);
    MeshComponent->SetupAttachment(RootComponent);
}

void ASpeckleUnrealStaticMesh::SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager)
{
	//TODO
	
}



void ASpeckleUnrealStaticMesh::SetMesh(const FString& StreamID, const FString& ObjectID, const TArray<FVector>& Vertices, const TArray<TArray<TTuple<int32,int32>>>& Polygons, TArray<FVector2D>& TextureCoordinates, UMaterialInterface* Material, bool BuildSimpleCollision, bool UseFullBuild)
{
	// //Create Mesh Asset
	// FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle/Meshes"), StreamID, ObjectID);
	// UPackage* Package = CreatePackage(*PackagePath);
	//
	//
	// UStaticMesh* Mesh = NewObject<UStaticMesh>(Package, FName(ObjectID), RF_Public);
	//
	// Mesh->InitResources();
	// Mesh->SetLightingGuid();
	//
	// UStaticMeshDescription* StaticMeshDescription = Mesh->CreateStaticMeshDescription(RootComponent);
	// FMeshDescription& BaseMeshDescription = StaticMeshDescription->GetMeshDescription();
	//
	// //Build Settings
	// FStaticMeshSourceModel& SrcModel = Mesh->AddSourceModel();
	// SrcModel.BuildSettings.bRecomputeNormals = false;
	// SrcModel.BuildSettings.bRecomputeTangents = false;
	// SrcModel.BuildSettings.bRemoveDegenerates = false;
	// SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	// SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
	// SrcModel.BuildSettings.bGenerateLightmapUVs = true;
	// SrcModel.BuildSettings.SrcLightmapIndex = 0;
	// SrcModel.BuildSettings.DstLightmapIndex = 1;
	//
	// UStaticMesh::FBuildMeshDescriptionsParams MeshParams;
	// MeshParams.bBuildSimpleCollision = BuildSimpleCollision;
	// MeshParams.bCommitMeshDescription = true;
	// MeshParams.bMarkPackageDirty = true;
	// MeshParams.bUseHashAsGuid = false;
	//
	// //Set Mesh Data
	//
	// //const FName MaterialSlotName = Mesh->AddMaterial(Material);;
	// //BaseMeshDescription.PolygonGroupAttributes().RegisterAttribute<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName, 1, MaterialSlotName,  EMeshAttributeFlags::Transient);
	// {
	// 	const size_t NumberOfVertices = Vertices.Num();
	// 	StaticMeshDescription->ReserveNewVertices(NumberOfVertices);
	// 	
	// 	TMap<int32, FVertexID> VertexMap;
	// 	VertexMap.Reserve(NumberOfVertices);
	// 	
	// 	{
	// 		int32 i = 0;
	// 		for(const FVector VertexPosition : Vertices)
	// 		{
	// 			const FVertexID VertID = StaticMeshDescription->CreateVertex();
	// 			StaticMeshDescription->SetVertexPosition(VertID, VertexPosition);
	// 			VertexMap.Add(i, VertID);
	// 			i++;
	// 		}
	// 	}
	//
	//
	// 	//Convert Faces
	// 	const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();
	//
	// 	//StaticMeshDescription->SetPolygonGroupMaterialSlotName(PolygonGroupID, MaterialSlotName);
	// 	
	// 	StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 2, FVector2D::ZeroVector, EMeshAttributeFlags::None);
	//
	//
	// 	StaticMeshDescription->ReserveNewTriangles(Polygons.Num() * 3); //Reserve space assuming faces will all be triangles
	// 	StaticMeshDescription->ReserveNewPolygons(Polygons.Num());
	// 	StaticMeshDescription->ReserveNewVertexInstances(Polygons.Num() * 3); //Reserve space assuming faces will all be triangles
	//
	// 	for(int32 i = 0; i < Polygons.Num(); i ++)
	// 	{
	// 		TArray<TTuple<int32,int32>> ParsedPolygon = Polygons[i];
	// 		TArray<FVertexInstanceID> VertexInstances;
	// 		VertexInstances.Reserve(ParsedPolygon.Num());
	//
	// 		
	// 		TSet<FVertexID> Verts;
	// 		Verts.Reserve(ParsedPolygon.Num());
	// 		
	// 		for(const auto v : ParsedPolygon)
	// 		{
	// 			FVertexID Vert = VertexMap[v.Key];
	// 			bool AlreadyInSet;
	// 			Verts.Add(Vert, &AlreadyInSet);
	//
	// 			if(AlreadyInSet)
	// 			{
	// 				UE_LOG(LogTemp, Warning, TEXT("Invalid Polygon while creating mesh - vertex appears more than once in a face, duplicates will be ignored"));
	// 				continue;
	// 			}
	//
	// 			FVertexInstanceID VertexInstance = StaticMeshDescription->CreateVertexInstance(Vert);
	// 			
	// 			VertexInstances.Add(VertexInstance);
	// 			
	// 			StaticMeshDescription->SetVertexInstanceUV(VertexInstance, TextureCoordinates[v.Value]);
	// 		};
	//
	//
	// 		if(VertexInstances.Num() < 3)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Invalid Polygon while creating mesh - face has fewer than 3 verts, this face will be ignored"));
	// 			continue;
	// 		}
	// 		
	// 		TArray<FEdgeID> Edges;
	// 		Edges.Reserve(ParsedPolygon.Num());
	// 		
	// 		const FPolygonID PolygonID = StaticMeshDescription->CreatePolygon(PolygonGroupID, VertexInstances, Edges);
	//
	// 		for (const FEdgeID EdgeID : Edges)
	// 		{
	// 			StaticMeshDescription->GetEdgeHardnesses()[EdgeID] = true;
	// 		}
	// 		
	// 		StaticMeshDescription->ComputePolygonTriangulation(PolygonID);
	// 	}
	// 	
	// 	
	// 	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	// 	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	// 	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Binormal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	// 	BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Center, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
	// 	
	// 	FStaticMeshOperations::ComputePolygonTangentsAndNormals(BaseMeshDescription);
	// 	FStaticMeshOperations::ComputeTangentsAndNormals(BaseMeshDescription, EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents);
	// }
	//
	//
	//
	// //Mesh->PreEditChange(nullptr);
	// 		
	// Mesh->LightMapCoordinateIndex = SrcModel.BuildSettings.DstLightmapIndex;
	// Mesh->BuildFromMeshDescriptions(TArray<const FMeshDescription*>{&BaseMeshDescription}, MeshParams);
	// 	
	// if(UseFullBuild) Mesh->Build(true); //This makes conversion time much slower, but is needed for generating lightmap UVs
	// //Mesh->PostEditChange(); //This doesn't seem to be required
	//
	//
	// FAssetRegistryModule::AssetCreated(Mesh);
	//
	// MeshComponent->SetStaticMesh(Mesh);
	//
	// //MeshComponent->SetMaterialByName(MaterialSlotName, Material);
}
