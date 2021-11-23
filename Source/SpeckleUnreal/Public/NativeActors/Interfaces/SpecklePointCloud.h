// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpecklePointCloud.generated.h"

class ASpeckleUnrealManager;
class UPointCloud;

// This class does not need to be modified.
UINTERFACE()
class USpecklePointCloud : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECKLEUNREAL_API ISpecklePointCloud
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void SetData(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager);
	
};
