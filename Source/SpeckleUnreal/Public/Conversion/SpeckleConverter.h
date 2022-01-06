﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "SpeckleConverter.generated.h"

class UBase;
class ASpeckleUnrealManager;

// This class does not need to be modified.
UINTERFACE()
class USpeckleConverter : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Interfaces for conversion functions (ToSpeckle and ToNative) of a specific native type.
 */
class SPECKLEUNREAL_API ISpeckleConverter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool CanConvertToNative(const FString& SpeckleType);
	
	UFUNCTION(BlueprintNativeEvent)
	AActor* ConvertToNative(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager);

	UFUNCTION(BlueprintNativeEvent)
	UBase* ConvertToSpeckle(const UObject* Object, ASpeckleUnrealManager* Manager);
};

#define CONVERTS_SPECKLE_TYPES() \
protected: \
	UPROPERTY(EditAnywhere, BlueprintReadWrite) \
	TSet<FString> SpeckleTypes; \
public: \
	virtual bool CanConvertToNative_Implementation(const FString &SpeckleType) override { return SpeckleTypes.Contains(SpeckleType); } \
private: