// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/RenderMaterial.h"
#include "UObject/Object.h"
#include "MaterialConverter.generated.h"

/**
* 
*/
UCLASS()
class SPECKLEUNREAL_API UMaterialConverter : public UObject
{
	GENERATED_BODY()

	public:

	static void AssignPropertiesFromSpeckle(UMaterialInstanceDynamic* Material, const URenderMaterial* SpeckleMaterial);

	static URenderMaterial* ParseRenderMaterial(const TSharedPtr<FJsonObject> obj);

};