// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"
#include "UObject/Interface.h"

#include "SpeckleTypeConverter.generated.h"

class UBase;
class ASpeckleUnrealManager;

// This class does not need to be modified.
UINTERFACE()
class USpeckleTypeConverter : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Interfaces for object conversion functions (ToSpeckle and ToNative) of a specific (most likely single) native type.
 *  
 *	Classes implementing this function are responsible for converting one or more UBase types
 *	to a native AActor. (ToNative)
 *	And/Or
 *	Converting one or more AActor types to a UBase type.
 *  
 *  Note: This interface is not equivalent to ISpeckleConverter in the .NET SDK.
 */
class SPECKLEUNREAL_API ISpeckleTypeConverter
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