// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"

#include "StaticMeshConverter.generated.h"

class AStaticMeshActor;
class UMesh;
class URenderMaterial;

UCLASS()
class SPECKLEUNREAL_API UStaticMeshConverter : public UObject, public ISpeckleConverter
{
	GENERATED_BODY()

	CONVERTS_SPECKLE_TYPES()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MeshActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseFullBuild;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool GenerateLightmapUV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BuildSimpleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Transient;

	
public:
	// Sets default values for this actor's properties
	UStaticMeshConverter();

	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;

	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;

	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshToNative(UObject* Outer, const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager);

	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshesToNative(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UMesh* MeshToSpeckle(const UStaticMeshComponent* Object, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager);
	
	virtual AActor* CreateEmptyActor(const ASpeckleUnrealManager* Manager, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

protected:
	virtual void GenerateMeshParams(UStaticMesh::FBuildMeshDescriptionsParams& MeshParams) const;
};
