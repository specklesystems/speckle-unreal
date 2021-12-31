// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpeckleTypeConverter.generated.h"

class UBase;
class ASpeckleUnrealManager;
class UMesh;

// This class does not need to be modified.
UINTERFACE()
class USpeckleTypeConverter : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Interfaces for conversion functions (ToSpeckle and ToNative) for a specific Speckle Type.
 */
class SPECKLEUNREAL_API ISpeckleTypeConverter
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	AActor* ConvertToNative(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager);

	UFUNCTION(BlueprintNativeEvent)
	UBase* ConvertToSpeckle(const UObject* Object, ASpeckleUnrealManager* Manager);
};
