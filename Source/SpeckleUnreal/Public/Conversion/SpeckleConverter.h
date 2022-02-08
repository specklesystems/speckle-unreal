// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"
#include "UObject/Interface.h"

#include "SpeckleConverter.generated.h"

class UBase;
class ASpeckleUnrealManager;

// This class does not need to be modified.
UINTERFACE()
class SPECKLEUNREAL_API USpeckleConverter : public UInterface
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
	bool CanConvertToNative(TSubclassOf<UBase> BaseType);

	/// Returns the type of Base expected for a given SpeckleType
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UBase> ToNativeBase(const FString& SpeckleType);
	
	UFUNCTION(BlueprintNativeEvent)
	AActor* ConvertToNative(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager);

	UFUNCTION(BlueprintNativeEvent)
	UBase* ConvertToSpeckle(const UObject* Object, ASpeckleUnrealManager* Manager);
};

#define CONVERTS_SPECKLE_TYPES() \
protected: \
	UPROPERTY(EditAnywhere, BlueprintReadWrite) \
	TSet<TSubclassOf<UBase>> SpeckleTypes; \
public: \
	virtual bool CanConvertToNative_Implementation(TSubclassOf<UBase> BaseType) override { return SpeckleTypes.Contains(BaseType); } \
private: