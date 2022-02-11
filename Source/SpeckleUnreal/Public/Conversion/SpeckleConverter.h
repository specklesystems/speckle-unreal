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
	/// Will return true if this converter can convert a given BaseType
	UFUNCTION(BlueprintNativeEvent)
	bool CanConvertToNative(TSubclassOf<UBase> BaseType);
	
	/// Tries to convert a given SpeckleBase into a native Actor
	UFUNCTION(BlueprintNativeEvent)
	AActor* ConvertToNative(const UBase* SpeckleBase, UWorld* World);

	/// Tries to convert a given Actor or Component Object into a Speckle Base
	UFUNCTION(BlueprintNativeEvent)
	UBase* ConvertToSpeckle(const UObject* Object);

	/// Clean up any cached assets that now may be unused
	UFUNCTION(BlueprintNativeEvent)
	void CleanUp();
	
};

#define CONVERTS_SPECKLE_TYPES() \
protected: \
	UPROPERTY(EditAnywhere, BlueprintReadWrite) \
	TSet<TSubclassOf<UBase>> SpeckleTypes; \
public: \
	virtual bool CanConvertToNative_Implementation(TSubclassOf<UBase> BaseType) override { return SpeckleTypes.Contains(BaseType); } \
private: