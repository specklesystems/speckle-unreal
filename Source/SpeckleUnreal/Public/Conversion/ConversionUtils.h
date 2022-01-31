// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConversionUtils.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UConversionUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Speckle|Conversion Utilities")
	static FMatrix TransformToNative(const TArray<float>& TransformData);

	//Parses units string into 
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetUnitsScaleFactorF(const FString& Units, const float WorldToCentimeters = 1);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetUnitsScaleFactor(const FString& Units, const UWorld* World = nullptr);
	
	//Safely try and get the WorldToCentimeters unit scale factor from the given world.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool TryGetWorldUnits(const UWorld* World, int32& OutWorldToCentimeters);
};
