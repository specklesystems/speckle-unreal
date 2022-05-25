// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"

#include "ObjectModelRegistry.generated.h"

class UBase;

/**
 * Handles the mapping of Speckle type to Object Model 
 */
UCLASS()
class SPECKLEUNREAL_API UObjectModelRegistry : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
private:

	static TMap<FString, TSubclassOf<UBase>> TypeRegistry;

	static void GenerateTypeRegistry();


public:

	/// Attempts to find the closest registered TSubclassOf<UBase>
	/// by recursively stripping away a the most specific name specifier from the given SpeckleType
	/// until a UBase type is found or the FString is exhausted.
	///  
	/// Eg. with an input of "Objects.Elements.Wall"
	/// Will first look for a registered type of "Objects.Elements.Wall"
	///	If one is not found, will look for "Objects.Elements" etc.
	///	Returns nullptr if none found.
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> FindClosestType(const FString& SpeckleType);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static bool ParentType(const FString& Type, FString& NextType);

	/// Attempts to find a TSubclassOf<UBase> with a UBase::SpeckleType matching the given SpeckleType param
	///	Returns nullptr if none found.
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> GetRegisteredType(const FString& SpeckleType);

	/// Attempts to find a TSubclassOf<UBase> with a UBase::SpeckleType matching the given SpeckleType param
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static bool TryGetRegisteredType(const FString& SpeckleType, TSubclassOf<UBase>& OutType);
};