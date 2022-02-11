// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConversionUtils.generated.h"

class ITransport;

/**
 *  Several conversion helper functions
 */
UCLASS()
class SPECKLEUNREAL_API UConversionUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField, const TScriptInterface<ITransport> Transport);
    
    static bool ResolveReference(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, TSharedPtr<FJsonObject>& OutObject);

	UFUNCTION()
	static float ParseScaleFactor(const FString& UnitsString);

};
