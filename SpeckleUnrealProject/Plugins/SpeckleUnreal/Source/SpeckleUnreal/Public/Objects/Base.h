// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Dom/JsonObject.h"

#include "Base.generated.h"

class ASpeckleUnrealManager;

/**
 * 
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API UBase : public UObject
{
public:
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Id;
	
	//UPROPERTY(BlueprintReadOnly)
	//TMap<FString, FString> Properties; //TODO figure out how I'm going to do custom properties

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SpeckleType;
	
	virtual void Deserialize(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* _)
	{
		Obj->TryGetStringField("id", Id);
		Obj->TryGetStringField("units", Units);
		Obj->TryGetStringField("speckle_type", SpeckleType);
	}
};