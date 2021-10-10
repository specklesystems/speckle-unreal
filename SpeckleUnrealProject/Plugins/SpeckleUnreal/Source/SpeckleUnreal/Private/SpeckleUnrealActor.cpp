// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleUnrealActor.h"

#include "MeshDescriptionBase.h"
#include "StaticMeshDescription.h"
#include "MeshTypes.h"
#include "StaticMeshOperations.h"

// Sets default values
ASpeckleUnrealActor::ASpeckleUnrealActor()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Scene;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComponent->SetupAttachment(RootComponent);

}






void ASpeckleUnrealActor::SetMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, TArray<FVector2D>& UV0, UMaterialInterface* Material)
{
	
	// Vertices[0] = FVector( 1.0f, -1.0f,  1.0f);
	// Vertices[1] = FVector( 1.0f,  1.0f,  1.0f);
	// Vertices[2] = FVector(-1.0f,  1.0f,  1.0f);
	// Vertices[3] = FVector(-1.0f, -1.0f,  1.0f);
	// Vertices[4] = FVector(-1.0f,  1.0f, -1.0f);
	// Vertices[5] = FVector(-1.0f, -1.0f, -1.0f);
	// Vertices[6] = FVector( 1.0f, -1.0f, -1.0f);
	// Vertices[7] = FVector( 1.0f,  1.0f, -1.0f);
	
	UStaticMeshDescription* StaticMeshDescription = NewObject<UStaticMeshDescription>();

	{
		const size_t NumberOfVertices = Vertices.Num();
		StaticMeshDescription->ReserveNewVertices(NumberOfVertices);
		
		TMap<int32, FVertexID> VertexMap;
		VertexMap.Reserve(NumberOfVertices);
		
		//Convert Vertices
		int32 i = 0;
		for(const FVector VertexPosition : Vertices)
		{
			const FVertexID VertID = StaticMeshDescription->CreateVertex();
			StaticMeshDescription->SetVertexPosition(VertID, VertexPosition);
			VertexMap.Add(i, VertID);
			i++;
		}

		//Convert Faces
		const FPolygonGroupID PolygonGroupID = StaticMeshDescription->CreatePolygonGroup();
		
		//For now, we assume triangles, however, this should work for polygons with any number of verts.
		for(int32 VertIndex = 0; VertIndex < (Triangles.Num() - 2); VertIndex += 3)
		{
			TArray<FVertexInstanceID> VertexInstances
			{
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i]]),
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i+1]]),
				StaticMeshDescription->CreateVertexInstance(VertexMap[Triangles[i+2]])
			};

			//TVertexInstanceAttributesRef<FVector2D> UVs = StaticMeshDescription->SetVertexInstanceUV();
			//TODO set UVS
			
			TArray<FEdgeID> Edges;
			Edges.Reserve(3);
			
			StaticMeshDescription->CreatePolygon(PolygonGroupID, VertexInstances, Edges);
		}
		
		FMeshDescription BaseMeshDescription = StaticMeshDescription->GetMeshDescription();


		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Normal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Tangent, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Binormal, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		BaseMeshDescription.PolygonAttributes().RegisterAttribute<FVector>(MeshAttribute::Polygon::Center, 1, FVector::ZeroVector, EMeshAttributeFlags::Transient);
		
		FStaticMeshOperations::ComputePolygonTangentsAndNormals(BaseMeshDescription);
		FStaticMeshOperations::ComputeTangentsAndNormals(BaseMeshDescription, EComputeNTBsFlags::Normals | EComputeNTBsFlags::Tangents);

	}


	//TODO look at CreateCube, Vertex instances need to be unique to polygon
	//Also some extra stuff I need to do with tangents
	
	UStaticMesh* mesh = NewObject<UStaticMesh>(MeshComponent);
	
	mesh->BuildFromStaticMeshDescriptions(TArray<UStaticMeshDescription*>{StaticMeshDescription});

	MeshComponent->SetStaticMesh(mesh);
}

// Called when the game starts or when spawned
void ASpeckleUnrealActor::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ASpeckleUnrealActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


