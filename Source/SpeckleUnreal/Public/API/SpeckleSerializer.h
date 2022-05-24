// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Objects/HighLevel/SpeckleStream.h"
#include "Objects/HighLevel/SpeckleBranch.h"
#include "Objects/HighLevel/SpeckleCommit.h"
#include "Objects/HighLevel/SpeckleUser.h"
#include "Objects/HighLevel/SpeckleGlobals.h"


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

	static TArray<FSpeckleBranch> DeserializeListOfBranches(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);

	static TArray<FSpeckleCommit> DeserializeListOfCommits(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);

	static FSpeckleUser DeserializeMyUserData(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);

	static FSpeckleGlobals DeserializeGlobals(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);

	static void DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj);
};
