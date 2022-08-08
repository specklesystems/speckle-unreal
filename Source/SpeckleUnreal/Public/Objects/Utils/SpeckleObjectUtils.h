
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "SpeckleObjectUtils.generated.h"

class ITransport;
class UBase;
class FJsonValue;
class FJsonObject;
class UWorld;
class AActor;

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

	
	static bool ParseVector(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, FVector& OutObject);
	static bool ParseVectorProperty(const TSharedPtr<FJsonObject> Base, const FString& PropertyName, const TScriptInterface<ITransport> ReadTransport, FVector& OutObject);

	template <typename T>
	static bool ParseSpeckleObject(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, T*& OutObject);


	UFUNCTION(BlueprintCallable, Category="Speckle/ObjectUtils")
	static float ParseScaleFactor(const FString& UnitsString);

	// Given a Right Handed Z-up transformation matrix (Speckle's system), will create an equivalent Left Handed Z-up FTransform (UE's system)
	UFUNCTION(BlueprintPure, Category="Speckle/ObjectUtils")
	static FTransform CreateTransform(UPARAM(ref) const FMatrix& TransformMatrix);

	UFUNCTION(BlueprintCallable, Category="Speckle/ObjectUtils", meta=(CallableWithoutWorldContext, WorldContext="WorldContextObject", DeterminesOutputType="Class", DynamicOutputParam="ReturnValue"))
	static AActor* SpawnActorInWorld(const UObject* WorldContextObject, const TSubclassOf<AActor> Class, UPARAM(ref) const FTransform& Transform);

	// Helper function to print a json obj to console
	static FString DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj);
	
	
};
