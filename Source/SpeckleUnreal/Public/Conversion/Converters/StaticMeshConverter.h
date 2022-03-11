// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "Engine/EngineTypes.h"

#include "StaticMeshConverter.generated.h"

class UMaterialConverter;
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
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>& AvailableConverters) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;
	virtual void CleanUp_Implementation() override;
	
	// Converts a multiple Speckle Meshes to a native actor of type MeshActorType
	UFUNCTION(BlueprintCallable)
	virtual AActor* MeshesToNativeActor(const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, UWorld* World, TScriptInterface<ISpeckleConverter>& RenderMaterialConverter);

	// Converts a single Speckle Mesh to a native actor of type MeshActorType
	UFUNCTION(BlueprintCallable)
	virtual AActor* MeshToNativeActor(const UMesh* SpeckleMesh, UWorld* World, TScriptInterface<ISpeckleConverter>& MaterialConverter);
	
	virtual AActor* CreateEmptyActor(UWorld* World, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters =
		                                 FActorSpawnParameters());
	
	UFUNCTION(BlueprintCallable)
	virtual UBase* MeshToSpeckle(const UStaticMeshComponent* Object);

	
protected:
	virtual void GenerateMeshParams(UStaticMesh::FBuildMeshDescriptionsParams& MeshParams) const;
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, UWorld* World,
	                                        TScriptInterface<ISpeckleConverter>& MaterialConverter) const;
	
	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshesToNativeMesh(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, TScriptInterface<ISpeckleConverter>
	                                        & MaterialConverter);
	UFUNCTION(BlueprintCallable)
	virtual UStaticMesh* MeshToNativeMesh(UObject* Outer, const UMesh* SpeckleMesh, TScriptInterface<ISpeckleConverter>& MaterialConverter);
};
