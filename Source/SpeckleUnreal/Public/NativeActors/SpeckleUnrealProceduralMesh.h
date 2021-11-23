// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "SpeckleUnrealActor.h"
#include "Interfaces/SpeckleMesh.h"

#include "SpeckleUnrealProceduralMesh.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealProceduralMesh : public ASpeckleUnrealActor, public ISpeckleMesh
{
	GENERATED_BODY()

protected:
	UMaterialInterface* CreateMaterial(TSharedPtr<FJsonObject> RenderMaterialObject, bool AcceptMaterialOverride = true);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* MeshComponent;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealProceduralMesh();
	
	virtual void SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager) override;
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager);
};
