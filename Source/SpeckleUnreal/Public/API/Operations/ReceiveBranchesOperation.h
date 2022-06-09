// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "API/Models/SpeckleBranch.h"

#include "ReceiveBranchesOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveBranchesOperationHandler,
											const TArray<FSpeckleBranch>&, Branches,
											const FString&, ErrorMessage);

/**
 *   Receive All branches on a given stream (up to a given limit)
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveBranchesOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FReceiveBranchesOperationHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FReceiveBranchesOperationHandler OnError;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations", meta = (WorldContext = "WorldContextObject"))
	static UReceiveBranchesOperation* ReceiveBranchesOperation(UObject* WorldContextObject,
															const FString& ServerUrl, const FString& AuthToken, const FString& StreamId, int32 Limit = 20);
	virtual void Activate() override;
	
protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;
	FString StreamId;
	int32 Limit;

	void HandleReceive(const FString& ResponseJson);
	
	void HandleError(const FString& Message);
};
