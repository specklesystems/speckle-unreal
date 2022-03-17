#pragma once

#include "SpeckleSerializer.generated.h"

class UBase;
class ITransport;

UCLASS()
class USpeckleSerializer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category="Speckle|Serialization")
	static UBase* DeserializeBaseById(const FString& ObjectId, const TScriptInterface<ITransport> ReadTransport);

	static UBase* DeserializeBase(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);


};
