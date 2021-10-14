// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeckleUnrealMesh.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "StaticMeshOperations.h"


// Sets default values
ASpeckleUnrealMesh::ASpeckleUnrealMesh() : ASpeckleUnrealActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

    MeshComponent = NewObject<UStaticMeshComponent>(RootComponent);
    MeshComponent->SetMobility(EComponentMobility::Stationary);
    MeshComponent->SetupAttachment(RootComponent);
}

void ASpeckleUnrealMesh::SetMesh(const TArray<FVector>& Vertices, const TArray<TArray<TTuple<int32,int32>>>& Polygons, TArray<FVector2D>& TextureCoordinates, UMaterialInterface* Material)
{
	UStaticMesh* Mesh = NewObject<UStaticMesh>(MeshComponent, "Mesh", RF_Public);
	Mesh->InitResources();

	Mesh->LightingGuid = FGuid::NewGuid();
	
	UStaticMeshDescription* StaticMeshDescription = Mesh->CreateStaticMeshDescription(RootComponent);
	FMeshDescription& BaseMeshDescription = StaticMeshDescription->GetMeshDescription();
	
	const FName MaterialSlotName = Mesh->AddMaterial(Material);;
	BaseMeshDescription.PolygonGroupAttributes().RegisterAttribute<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName, 1, MaterialSlotName,  EMeshAttributeFlags::Transient);
	
	{
		const size_t NumberOfVertices = Vertices.Num();
		StaticMeshDescription->ReserveNewVertices(NumberOfVertices);
		
		TMap<int32, FVertexID> VertexMap;
		VertexMap.Reserve(NumberOfVertices);
		
		{
			int32 i = 0;
			for(const FVector VertexPosition : Vertices)
			{
				const FVertexID VertID = StaticMeshDescription->CreateVertex();
				StaticMeshDescription->SetVertexPosition(VertID, VertexPosition);
				VertexMap.Add(i, VertID);
				i++;
			}
		}


		//Convert Faces
		const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();

		StaticMeshDescription->SetPolygonGroupMaterialSlotName(PolygonGroupID, MaterialSlotName);
		
		StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 2, FVector2D::ZeroVector, EMeshAttributeFlags::Transient);


		StaticMeshDescription->ReserveNewTriangles(Polygons.Num() * 3); //Reserve space assuming faces will all be triangles
		StaticMeshDescription->ReserveNewPolygons(Polygons.Num());
		StaticMeshDescription->ReserveNewVertexInstances(Polygons.Num() * 3); //Reserve space assuming faces will all be triangles

		for(int32 i = 0; i < Polygons.Num(); i ++)
		{
			TArray<TTuple<int32,int32>> ParsedPolygon = Polygons[i];
			TArray<FVertexInstanceID> VertexInstances;
			VertexInstances.Reserve(ParsedPolygon.Num());
			
			for(const auto v : ParsedPolygon)
			{
				FVertexInstanceID VertexInstance = StaticMeshDescription->CreateVertexInstance(VertexMap[v.Key]);
				VertexInstances.Add(VertexInstance);
				
				StaticMeshDescription->SetVertexInstanceUV(VertexInstance, TextureCoordinates[v.Value]);
			};
			
			TArray<FEdgeID> Edges;
			Edges.Reserve(ParsedPolygon.Num());
			
			const FPolygonID PolygonID = StaticMeshDescription->CreatePolygon(PolygonGroupID, VertexInstances, Edges);

			for (const FEdgeID EdgeID : Edges)
			{
				StaticMeshDescription->GetEdgeHardnesses()[EdgeID] = true;
			}
			
			StaticMeshDescription->ComputePolygonTriangulation(PolygonID);
		}
		
		
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Binormal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Center, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		
		FStaticMeshOperations::ComputePolygonTangentsAndNormals(BaseMeshDescription);
		FStaticMeshOperations::ComputeTangentsAndNormals(BaseMeshDescription, EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents);
	}
	
	FStaticMeshSourceModel& SrcModel = Mesh->AddSourceModel();
	SrcModel.BuildSettings.bRecomputeNormals = false;
	SrcModel.BuildSettings.bRecomputeTangents = false;
	SrcModel.BuildSettings.bRemoveDegenerates = false;
	SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
	SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
	SrcModel.BuildSettings.bGenerateLightmapUVs = true;
	SrcModel.BuildSettings.SrcLightmapIndex = 0;
	SrcModel.BuildSettings.DstLightmapIndex = 1;
	
	
	Mesh->BuildFromStaticMeshDescriptions(TArray<UStaticMeshDescription*>{StaticMeshDescription});
	Mesh->CommitMeshDescription(0);
	Mesh->LightMapCoordinateIndex = SrcModel.BuildSettings.DstLightmapIndex;
	
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetMaterialByName(MaterialSlotName, Material);
}
