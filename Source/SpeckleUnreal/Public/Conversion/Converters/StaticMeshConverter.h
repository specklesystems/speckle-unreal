// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "Engine/EngineTypes.h"

#include "StaticMeshConverter.generated.h"

class URenderMaterialConverter;
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
	URenderMaterialConverter* MaterialConverter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MeshActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EComponentMobility::Type> ActorMobility;

	// If true, will use the full Editor Only build process
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

	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World) override;

	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;

	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshToNative(UObject* Outer, const UMesh* SpeckleMesh);

	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshesToNative(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes);
	
	UFUNCTION(BlueprintCallable)
	virtual UMesh* MeshToSpeckle(const UStaticMeshComponent* Object);
	
	virtual AActor* CreateEmptyActor(UWorld* World, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters =
		                                 FActorSpawnParameters());

	virtual void CleanUp_Implementation() override;
	
protected:
	virtual void GenerateMeshParams(UStaticMesh::FBuildMeshDescriptionsParams& MeshParams) const;
};
