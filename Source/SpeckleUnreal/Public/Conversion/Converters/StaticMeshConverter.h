// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

#include "StaticMeshConverter.generated.h"

class UMaterialConverter;
class AStaticMeshActor;
class UMesh;
class URenderMaterial;


/**
 * Converts Speckle Mesh objects into native Actors with a StaticMesh component
 *
 * Can convert multiple Speckle Mesh objects (eg with different materials) in one StaticMesh  
 */
UCLASS()
class SPECKLEUNREAL_API UStaticMeshConverter : public UObject, public ISpeckleConverter
{
	GENERATED_BODY()

	CONVERTS_SPECKLE_TYPES()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TSubclassOf<AActor> MeshActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TEnumAsByte<EComponentMobility::Type> ActorMobility;

#if WITH_EDITORONLY_DATA
	// If true, will use the full Editor Only build process
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	bool UseFullBuild;

	// When true, will display FSlowTask progress of editor only build process
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative|EditorOnly")
	bool DisplayBuildProgressBar;

	// When true, will allow cancellation of FSlowTask progress of editor only build process
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative|EditorOnly")
	bool AllowCancelBuild;
#endif
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	bool BuildSimpleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	bool Transient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative")
	bool GenerateLightmapUV;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative")
	int32 MinLightmapResolution;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative")
	bool BuildReversedIndexBuffer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative")
    bool UseFullPrecisionUVs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="ToNative")
	bool RemoveDegeneratesOnBuild;
public:
	// Sets default values for this actor's properties
	UStaticMeshConverter();
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>& AvailableConverters) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;
	virtual void FinishConversion_Implementation() override;
	
	// Converts a multiple Speckle Meshes to a native actor of type MeshActorType
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual AActor* MeshesToNativeActor(const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, UWorld* World, TScriptInterface<ISpeckleConverter>& RenderMaterialConverter);

	// Converts a single Speckle Mesh to a native actor of type MeshActorType
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual AActor* MeshToNativeActor(const UMesh* SpeckleMesh, UWorld* World, TScriptInterface<ISpeckleConverter>& MaterialConverter);
	
	virtual AActor* CreateEmptyActor(UWorld* World, const FTransform& Transform = FTransform::Identity, const FActorSpawnParameters& SpawnParameters =
		                                 FActorSpawnParameters());
	
	UFUNCTION(BlueprintCallable, Category="ToSpeckle")
	virtual UBase* MeshToSpeckle(const UStaticMeshComponent* Object);

	
protected:

	FCriticalSection Lock_StaticMeshesToBuild;
	
	UPROPERTY(BlueprintReadWrite, Transient, Category="ToNative")
	TArray<UStaticMesh*> StaticMeshesToBuild;
	
	virtual void GenerateMeshParams(UStaticMesh::FBuildMeshDescriptionsParams& MeshParams) const;
	
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, UWorld* World,
	                                        TScriptInterface<ISpeckleConverter>& MaterialConverter) const;
	
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual UStaticMesh* MeshesToNativeMesh(UObject* Outer, const UBase* Parent, const TArray<UMesh*>& SpeckleMeshes, TScriptInterface<ISpeckleConverter>
	                                        & MaterialConverter);
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual UStaticMesh* MeshToNativeMesh(UObject* Outer, const UMesh* SpeckleMesh, TScriptInterface<ISpeckleConverter>& MaterialConverter);
};
