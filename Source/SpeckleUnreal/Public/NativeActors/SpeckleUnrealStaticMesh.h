// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeckleUnrealActor.h"
#include "Interfaces/SpeckleMesh.h"

#include "SpeckleUnrealStaticMesh.generated.h"

class URenderMaterial;
/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealStaticMesh : public ASpeckleUnrealActor, public ISpeckleMesh
{
	GENERATED_BODY()
	
protected:
	virtual UStaticMesh* MeshToNative(UObject* Outer, const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager);
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool UseFullBuild;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool BuildSimpleCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool Transient;
		
	// Sets default values for this actor's properties
	ASpeckleUnrealStaticMesh();


	virtual void SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager) override;
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager);
};
