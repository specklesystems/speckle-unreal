// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

#include "Base.generated.h"

class ITransport;
class ASpeckleUnrealManager;

/**
 * Base type that all Object Models inherit from
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API UBase : public UObject
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
	
	TMap<FString, TSharedPtr<FJsonValue>> DynamicProperties; //TODO this won't be serialised!

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString SpeckleType;
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
	{
		bool IsValid = false;
		DynamicProperties = Obj->Values;
		if(Obj->TryGetStringField("id", Id))
		{
			IsValid = true;
			DynamicProperties.Remove("id");
		}
		
		if(Obj->TryGetStringField("units", Units)) DynamicProperties.Remove("units");
		if(Obj->TryGetStringField("speckle_type", SpeckleType)) DynamicProperties.Remove("speckle_type");

		return IsValid;
	}

protected:

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	int32 RemoveDynamicProperty(UPARAM(ref) const FString& Key)
	{
		return DynamicProperties.Remove(Key);
	}

public:
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicString(UPARAM(ref) const FString& Key, FString& OutString) const
	{
		const TSharedPtr<FJsonValue> Value = DynamicProperties.FindRef(Key);
		if(Value == nullptr) return false;
		return Value->TryGetString(OutString);
	}

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicNumber(UPARAM(ref) const FString& Key, float& OutNumber) const
	{
		const TSharedPtr<FJsonValue> Value = DynamicProperties.FindRef(Key);
		if(Value == nullptr) return false;
		return Value->TryGetNumber(OutNumber);
	}

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicBool(UPARAM(ref) const FString& Key, bool& OutBool) const
	{
		const TSharedPtr<FJsonValue> Value = DynamicProperties.FindRef(Key);
		if(Value == nullptr) return false;
		return Value->TryGetBool(OutBool);
	}
};

