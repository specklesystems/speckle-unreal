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
    MeshComponent->SetMobility(EComponentMobility::Static);
    MeshComponent->SetupAttachment(RootComponent);
}

void ASpeckleUnrealMesh::SetMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, TArray<FVector2D>& UV0, UMaterialInterface* Material)
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
		UV0.SetNum(NumberOfVertices); //Fill missing UVs with zeroed vectors.
		
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
		
		StaticMeshDescription->VertexInstanceAttributes().RegisterAttribute<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate, 1, FVector2D::ZeroVector, EMeshAttributeFlags::Transient);

		const int32 NumberOfTriangles = Triangles.Num() / 3;
		StaticMeshDescription->ReserveNewTriangles(NumberOfTriangles);
		StaticMeshDescription->ReserveNewPolygons(NumberOfTriangles);
		StaticMeshDescription->ReserveNewVertexInstances(NumberOfTriangles * 3);
		
		for(int32 i = 0; i < (Triangles.Num() - 2); i += 3)
		{
			TArray<FVertexInstanceID> VertexInstances
			{
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i]]),
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i+1]]),
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i+2]])
			};

			StaticMeshDescription->SetVertexInstanceUV(VertexInstances[0], UV0[Triangles[i]]);
			StaticMeshDescription->SetVertexInstanceUV(VertexInstances[1], UV0[Triangles[i+1]]);
			StaticMeshDescription->SetVertexInstanceUV(VertexInstances[2], UV0[Triangles[i+2]]);
			
			TArray<FEdgeID> Edges;
			Edges.Reserve(3);
			
			const FPolygonID PolygonID = StaticMeshDescription->CreatePolygon(PolygonGroupID, VertexInstances, Edges);

			for (const FEdgeID EdgeID : Edges)
			{
				StaticMeshDescription->GetEdgeHardnesses()[EdgeID] = true;
			}
			
			StaticMeshDescription->ComputePolygonTriangulation(PolygonID); //TODO might be a more efficient way to do this (since we are already a triangle)
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
	
	MeshComponent->SetStaticMesh(Mesh);
	MeshComponent->SetMaterialByName(MaterialSlotName, Material);
}
