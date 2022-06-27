// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0


#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "SpeckleAPIFunctions.generated.h"

struct FSpeckleCommit;
/**
 * Blueprint function library for Speckle API 
 */
UCLASS()
class SPECKLEUNREAL_API USpeckleAPIFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, CustomThunk, Category="Speckle|API", meta=(CustomStructureParam="OutStruct", ExpandBoolAsExecs="ReturnValue"))
	static UPARAM(DisplayName = "WasSucessful") bool DeserializeResponse(const FString& JsonString, int32& OutStruct);
	DECLARE_FUNCTION(execDeserializeResponse);
	
	static bool GenericDeserializeResponse(const FString& JsonString, const UScriptStruct* StructType, void* OutStruct);

	static void CommitReceived(FSpeckleCommit& Commit);


	
};
