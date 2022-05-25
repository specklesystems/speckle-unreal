// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicBase.h"
#include "Dom/JsonObject.h"

#include "Base.generated.h"

class ITransport;
class ASpeckleUnrealManager;

/**
 * Base type that all Object Models inherit from
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API UBase : public UDynamicBase
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString SpeckleType;
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
	{
		bool IsValid = false;
		DynamicProperties.Reserve(Obj->Values.Num());
		for(const auto kvp : Obj->Values)
		{
			SetDynamicProperty(kvp.Key, kvp.Value->Type);
		}

		if(Obj->TryGetStringField("id", Id))
		{
			IsValid = true;
			DynamicProperties.Remove("id");
		}
		
		if(Obj->TryGetStringField("units", Units)) DynamicProperties.Remove("units");
		if(Obj->TryGetStringField("speckle_type", SpeckleType)) DynamicProperties.Remove("speckle_type");

		return IsValid;
	}


	virtual void ToJson(TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> Writer)
	{
		Writer.WriteValue(TEXT("units"), Units);
		Writer.WriteValue(TEXT("speckle_type"), SpeckleType);

		for(const auto& p : DynamicProperties)
		{
			//Writer.WriteValue(p.Key, p.Value.mmmmmm));
		}
	}
	


};
