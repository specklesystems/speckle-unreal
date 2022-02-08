// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"

#include "ProceduralMeshConverter.generated.h"

class URenderMaterialConverter;
class UProceduralMeshComponent;
class UMesh;
class URenderMaterial;

UCLASS()
class SPECKLEUNREAL_API UProceduralMeshConverter :  public UObject, public ISpeckleConverter
{
	GENERATED_BODY()

	CONVERTS_SPECKLE_TYPES()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URenderMaterialConverter* MaterialConverter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MeshActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EComponentMobility::Type> ActorMobility;
	
	// Sets default values for this actor's properties
	UProceduralMeshConverter();
	
	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;
	
	UFUNCTION(BlueprintCallable)
	virtual AActor* MeshToNative(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UMesh* MeshToSpeckle(const UProceduralMeshComponent* Object, ASpeckleUnrealManager* Manager);
	
	virtual AActor* CreateEmptyActor(const ASpeckleUnrealManager* Manager, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

	virtual void CleanUp_Implementation() override;
};
