// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "UObject/Object.h"
#include "MaterialConverter.generated.h"

class URenderMaterial;
class ASpeckleUnrealManager;

UENUM()
enum EConstMaterialOptions
{
	  Never UMETA(DisplayName = "Never"),
	  NotPlay UMETA(DisplayName = "Editor Not Playing"),
	  Always UMETA(DisplayName = "Editor And PIE"),
};


/**
 *  Converts Speckle RenderMaterial objects into native Materials
 */
UCLASS(BlueprintType, Blueprintable)
class SPECKLEUNREAL_API UMaterialConverter : public UObject, public ISpeckleConverter
{
	GENERATED_BODY()

	CONVERTS_SPECKLE_TYPES()
	
public:

	/// Material to be applied to meshes when no RenderMaterial can be converted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* DefaultMeshMaterial;

	/// Material Parent for converted opaque materials*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* BaseMeshOpaqueMaterial;

	/// Material Parent for converted materials with an opacity less than one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	UMaterialInterface* BaseMeshTransparentMaterial;

#if WITH_EDITORONLY_DATA
	/// Specify when to create Constant materials that can only be created with Editor.
	/// Otherwise will create dynamic materials
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	TEnumAsByte<EConstMaterialOptions> UseConstMaterials;
#endif
	
	/// When generating meshes, materials in this TMap will be used
	/// instead of converted ones if the key matches the ID of the Object's RenderMaterial. (Takes priority over name matching)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overrides", DisplayName = "By Speckle ID")
	TMap<FString, UMaterialInterface*> MaterialOverridesById;

	/// When generating meshes, materials in this TSet will be used instead of converted ones if the material name matches the name of the Object's RenderMaterial
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overrides", DisplayName = "By Name")
	TSet<UMaterialInterface*> MaterialOverridesByName;

public:

	UMaterialConverter();
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld*, TScriptInterface<ISpeckleConverter>&) override;

	UFUNCTION(BlueprintCallable)
	virtual bool TryGetOverride(const URenderMaterial* SpeckleMaterial, UMaterialInterface*& OutMaterial) const;

	

	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetMaterial(const URenderMaterial* SpeckleMaterial);
	
	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* RenderMaterialToNative(const URenderMaterial* SpeckleMaterial, UPackage* Package);

	UFUNCTION(BlueprintCallable)
	virtual void CleanUp_Implementation() override;
	
protected:
	
	/** Transient cache of materials converted from stream RenderMaterial objects */
	UPROPERTY(AdvancedDisplay, BlueprintReadOnly, Transient, Category = "Conversion")
	TMap<FString, UMaterialInterface*> ConvertedMaterials;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual UPackage* GetPackage(const FString& ObjectID) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FString RemoveInvalidFileChars(const FString& InString) const;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ShouldCreateConstMaterial(TEnumAsByte<EConstMaterialOptions> Options);
#endif
	
};
