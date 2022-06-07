// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleStream.h"

#include "ReceiveStreamsOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveStreamsOperationHandler, const TArray<FSpeckleStream>&, Streams, FString, ErrorMessage);

/**
 *   Receive All streams	
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveStreamsOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FReceiveStreamsOperationHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FReceiveStreamsOperationHandler OnError;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations", meta = (WorldContext = "WorldContextObject"))
	static UReceiveStreamsOperation* ReceiveStreamsOperation(UObject* WorldContextObject, const FString& ServerUrl, const FString& AuthToken, const int32 Limit = 20);
	virtual void Activate() override;
	
protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;
	int32 Limit;

	void HandleReceive(const TArray<FSpeckleStream>& Streams);
	
	void HandleError(const FString& Message);
};
