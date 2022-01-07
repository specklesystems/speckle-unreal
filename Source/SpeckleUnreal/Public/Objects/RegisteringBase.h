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
	static TOptional<TMap<FString, TSubclassOf<UBase>>> TypeRegistry;

	static void GenerateTypeRegistry();

public:
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> GetRegisteredType(const FString& SpeckleType);
};