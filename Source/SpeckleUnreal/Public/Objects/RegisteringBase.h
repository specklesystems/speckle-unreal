// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RegisteringBase.generated.h"

class UBase;

UCLASS(Abstract)
class SPECKLEUNREAL_API URegisteringBase : public UObject
{
	GENERATED_BODY()
	
protected:
	//static TOptional<TMap<FString, TSubclassOf<UBase>>> TypeRegistry;
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

	/// Attempts to find a TSubclassOf<UBase> with a UBase::SpeckleType matching the given SpeckleType param
	///	Returns nullptr if none found.
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> GetRegisteredType(const FString& SpeckleType);

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static bool TryGetRegisteredType(const FString& SpeckleType, TSubclassOf<UBase>& OutType);
};