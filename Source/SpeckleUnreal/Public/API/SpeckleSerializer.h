#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Objects/HighLevel/SpeckleStream.h"


#include "SpeckleSerializer.generated.h"

class UBase;
class ITransport;
class FJsonObject;

UCLASS()
class USpeckleSerializer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Speckle|Serialization")
	static UBase* DeserializeBaseById(const FString& ObjectId, const TScriptInterface<ITransport> ReadTransport);

	static UBase* DeserializeBase(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);

	static TArray<FSpeckleStream> DeserializeListOfStreams(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);
};
