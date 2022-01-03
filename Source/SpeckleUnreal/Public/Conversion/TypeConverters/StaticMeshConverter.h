// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleTypeConverter.h"

#include "StaticMeshConverter.generated.h"

class AStaticMeshActor;
class UMesh;
class URenderMaterial;

UCLASS()
class SPECKLEUNREAL_API UStaticMeshConverter : public UObject, public ISpeckleTypeConverter
{
	GENERATED_BODY()

protected:

	virtual AStaticMeshActor* CreateActor(const FTransform& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseFullBuild;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BuildSimpleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Transient;
	
	// Sets default values for this actor's properties
	UStaticMeshConverter();


	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;

	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshToNative(UObject* Outer, const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager);

	UFUNCTION(BlueprintCallable)
	virtual UMesh* MeshToSpeckle(const UStaticMeshComponent* Object, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager);
	
};
