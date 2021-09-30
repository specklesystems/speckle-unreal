// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleUnrealMesh.h"

// Sets default values
ASpeckleUnrealMesh::ASpeckleUnrealMesh()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Scene;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	ProceduralMesh->SetupAttachment(RootComponent);

}



void ASpeckleUnrealMesh::SetMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, TArray<FVector2D>& UV0, UMaterialInterface* Material)
{
	ProceduralMesh->ClearAllMeshSections();

	TArray<FVector> Normals;
	
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> Colors;

	// for each triangle
	for (size_t i = 0; i < Triangles.Num(); i += 3)
	{
		// get a normal direction for this triangle
		const FVector Normal = FVector::CrossProduct(Vertices[Triangles[i]], Vertices[Triangles[i + 2]]).GetSafeNormal();

		// get a tangent direction perpendicular to the normal
		const FVector TangentVector = FVector::CrossProduct(Vertices[Triangles[i]], Normal);
		const FProcMeshTangent Tangent = FProcMeshTangent(TangentVector.X, TangentVector.Y, TangentVector.Z);

		// for each vertex in a triangle
		for (size_t j = 0; j < 3; j++)
		{
			Normals.Add(Normal);
			Tangents.Add(Tangent);
			Colors.Add(FColor::White);
		}
	}

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, Colors, Tangents, true);

	ProceduralMesh->SetMaterial(0, Material);
}

// Called when the game starts or when spawned
void ASpeckleUnrealMesh::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASpeckleUnrealMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

