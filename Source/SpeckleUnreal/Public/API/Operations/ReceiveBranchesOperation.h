// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleBranch.h"

#include "ReceiveBranchesOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveBranchesOperationHandler,
											const TArray<FSpeckleBranch>&, Branches,
											const FString&, ErrorMessage);

/**
 *   Receive All streams	
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

	void HandleReceive(const TArray<FSpeckleBranch>& Branches);
	
	void HandleError(const FString& Message);
};
