// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "UObject/Object.h"
#include "RenderMaterialConverter.generated.h"

class URenderMaterial;
class ASpeckleUnrealManager;

/**
 * Class for converting URenderMaterial.
 * Note, this class is not a ISpeckleConverter, since it does not create an Actor. //TODO consider allowing ISpeckleConverter to return any UObject
 * Instead this class is used by ISpeckleConverter implementors.
 */
UCLASS(BlueprintType, Blueprintable)
class SPECKLEUNREAL_API URenderMaterialConverter : public UObject
{
	GENERATED_BODY()

public:

	URenderMaterialConverter();

	/** Material to be applied to meshes when no RenderMaterial can be converted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* DefaultMeshMaterial;

	/** Material Parent for converted opaque materials*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* BaseMeshOpaqueMaterial;

	/** Material Parent for converted materials with an opacity less than one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* BaseMeshTransparentMaterial;
	
	/** When generating meshes, materials in this TMap will be used instead of converted ones if the key matches the ID of the Object's RenderMaterial. (Takes priority over name matching)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overrides", DisplayName = "By Speckle ID")
	TMap<FString, UMaterialInterface*> MaterialOverridesById;

	/** When generating meshes, materials in this TSet will be used instead of converted ones if the material name matches the name of the Object's RenderMaterial. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overrides", DisplayName = "By Name")
	TSet<UMaterialInterface*> MaterialOverridesByName;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool TryGetOverride(const URenderMaterial* SpeckleMaterial, UMaterialInterface*& OutMaterial) const;

	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial, const ASpeckleUnrealManager* Manager, bool AcceptMaterialOverride = true, bool AllowEditorMaterial = false);
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* RenderMaterialToNative(const URenderMaterial* SpeckleMaterial, UPackage* Package, bool AllowEditorMaterial);

	UFUNCTION(BlueprintCallable)
	virtual void CleanUp();
	
protected:
	
	/** Transient cache of materials converted from stream RenderMaterial objects */
	UPROPERTY(AdvancedDisplay, BlueprintReadOnly, Transient, Category = "Conversion")
	TMap<FString, UMaterialInterface*> ConvertedMaterials;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual UPackage* GetPackage(const FString& StreamID, const FString& ObjectID) const;
};
