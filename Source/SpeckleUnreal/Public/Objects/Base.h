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
	
	explicit UBase(FString& SpeckleType) : UBase()
	{
		this->SpeckleType = SpeckleType;
	}
	
public:

	UBase() {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Id;
	
	//UPROPERTY(BlueprintReadOnly)
	//TMap<FString, FString> Properties; //TODO figure out how I'm going to do custom properties

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Speckle|Objects")
	FString SpeckleType;
	
	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* _)
	{
		Obj->TryGetStringField("id", Id);
		Obj->TryGetStringField("units", Units);
	}
};

