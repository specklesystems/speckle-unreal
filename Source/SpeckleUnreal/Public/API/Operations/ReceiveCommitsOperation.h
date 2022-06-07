// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleCommit.h"

#include "ReceiveCommitsOperation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveCommitsOperationHandler, const TArray<FSpeckleCommit>&, Commits, FString, ErrorMessage);


/**
 *   Receive All Commits	
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveCommitsOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FReceiveCommitsOperationHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FReceiveCommitsOperationHandler OnError;


	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations",
						meta = (WorldContext = "WorldContextObject", BranchName = "main"))
	static UReceiveCommitsOperation* ReceiveCommitsOperation(UObject* WorldContextObject,
																const FString& ServerUrl,
																const FString& AuthToken,
																const FString& StreamId,
																const FString& BranchName,
																const int32 Limit = 20);
	virtual void Activate() override;
	
protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;
	FString StreamId;
	FString BranchName;
	int32 Limit;

	void HandleReceive(const TArray<FSpeckleCommit>& Commits);
	
	void HandleError(const FString& Message);
};
