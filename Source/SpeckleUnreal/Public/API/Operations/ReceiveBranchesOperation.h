// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleBranch.h"

#include "ReceiveBranchesOperation.generated.h"


class ITransport;
class UBase;
class FJsonObject;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveBranchesOperationHandler,
											const TArray<FSpeckleBranch>&, Branches,
												FString, ErrorMessage);

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
															const FString& StreamId,
															TScriptInterface<ITransport> RemoteTransport,
															TScriptInterface<ITransport> LocalTransport);
	virtual void Activate() override;
	
protected:
	void Receive();
	
	FString StreamId;
	TScriptInterface<ITransport> RemoteTransport;
	TScriptInterface<ITransport> LocalTransport;

	void HandleReceive(TSharedPtr<FJsonObject> Object);
	
	void HandleError(FString& Message);
};
