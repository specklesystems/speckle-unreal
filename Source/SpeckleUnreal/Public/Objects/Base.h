// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/RegisteringBase.h"
#include "Dom/JsonObject.h"

#include "Base.generated.h"

class ASpeckleUnrealManager;

/**
 * 
 */


UCLASS(BlueprintType)
class SPECKLEUNREAL_API UBase : public URegisteringBase
{
public:

	GENERATED_BODY()
	
protected:
	
	explicit UBase(const wchar_t* SpeckleType): SpeckleType(SpeckleType) {}
	explicit UBase(const FString& SpeckleType) : SpeckleType(SpeckleType) {}
	
public:

	UBase() : SpeckleType("Base") {}


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Id;
	
	//UPROPERTY(BlueprintReadOnly)
	//TMap<FString, FString> Properties; //TODO figure out how I'm going to do custom properties

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString SpeckleType;

	//TODO this won't be serialised
	TMap<FString, TSharedPtr<FJsonValue>> DynamicProperties;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	TMap<FString, UBase*> Children;
	
	/// Callback called right before serialization of this object 
	virtual void OnBeforeSerialize();
	/// Callback called right after deserialization of this object
	virtual void OnAfterDeserialize();
};

