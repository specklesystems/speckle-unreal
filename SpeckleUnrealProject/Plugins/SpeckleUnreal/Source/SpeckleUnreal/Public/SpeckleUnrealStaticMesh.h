// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeckleMesh.h"
#include "SpeckleUnrealActor.h"

#include "SpeckleUnrealStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealStaticMesh : public ASpeckleUnrealActor, public ISpeckleMesh
{
	GENERATED_BODY()

protected:

	virtual UMaterialInterface* CreateMaterial(ASpeckleUnrealManager* Manager, const URenderMaterial& Material);
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealStaticMesh();


	virtual void SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager) override;
	
	
	virtual void SetMesh(
		const FString& StreamID,
		const FString& ObjectID,
		const TArray<FVector>& Vertices,
		const TArray<TArray<TTuple<int32,int32>>>& Polygons,
		TArray<FVector2D>& TextureCoordinates,
		UMaterialInterface* Material,
		bool BuildSimpleCollision = false,
		bool UseFullBuild = false
		);
};
