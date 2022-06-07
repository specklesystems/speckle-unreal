// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleGlobals.h"

#include "ReceiveGlobalsOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveGlobalsOperationHandler, const FSpeckleGlobals, Globals, FString, ErrorMessage);


/**
 *   Receive My User Data in Speckle
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveGlobalsOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FReceiveGlobalsOperationHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FReceiveGlobalsOperationHandler OnError;
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations", meta = (WorldContext = "WorldContextObject"))
	static UReceiveGlobalsOperation* ReceiveGlobalsOperation(UObject* WorldContextObject,
	                                                         const FString& ServerUrl, const FString& AuthToken,
	                                                         const FString& StreamId, const FString& ReferencedObjectId);
	virtual void Activate() override;

protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;
	FString StreamId;
	FString ReferencedObjectId;

	void HandleReceive(const FSpeckleGlobals& Object);
	
	void HandleError(const FString& Message);
};
