// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeckleObjectUtils.generated.h"

class ITransport;

/**
 *  Several helper functions useful for handling JSON Speckle Objects
 */
UCLASS()
class SPECKLEUNREAL_API USpeckleObjectUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField, const TScriptInterface<ITransport> Transport);
    
    static bool ResolveReference(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, TSharedPtr<FJsonObject>& OutObject);

	static bool TryParseTransform(const TSharedPtr<FJsonObject> SpeckleObject, FMatrix& OutMatrix);
	
	UFUNCTION(BlueprintCallable)
	static float ParseScaleFactor(const FString& UnitsString);
	
};
