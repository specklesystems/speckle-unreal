// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "API/Models/SpeckleStream.h"

#include "ReceiveStreamsOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveStreamsOperationHandler, const TArray<FSpeckleStream>&, Streams, FString, ErrorMessage);

/**
 * Receive all streams associated with the authenticated account (up to specified limit)
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

	void HandleReceive(const FString& ResponseJson);
	
	void HandleError(const FString& Message);
};
