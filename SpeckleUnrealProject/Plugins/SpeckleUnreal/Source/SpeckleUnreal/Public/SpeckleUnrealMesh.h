// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeckleUnrealActor.h"
#include "SpeckleUnrealMesh.generated.h"

UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealMesh : public ASpeckleUnrealActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealMesh();

	virtual void SetMesh(const TArray<FVector>& Vertices, const TArray<TArray<TTuple<int32,int32>>>& Polygons, TArray<FVector2D>& TextureCoordinates, UMaterialInterface* Material);
};
