// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "ReceiveOperation.generated.h"

class ITransport;
class UBase;
class FJsonObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRecieveOperationHandler, UBase*, RootBase, FString, ErrorMessage);

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FRecieveOperationHandler OnReceiveSuccessfully;

	/// Called when the total number of children is known
    //UPROPERTY(BlueprintAssignable)
    //FRecieveOperationHandler OnChildrenCountKnown;

	/// Called when some deserilization progress is made and TotalConverted has changed
    //UPROPERTY(BlueprintAssignable)
    //FRecieveOperationHandler OnProgress;

	/// Called when receive operation has aborted due to some error
    UPROPERTY(BlueprintAssignable)
    FRecieveOperationHandler OnError;

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations", meta = (WorldContext = "WorldContextObject"))
	static UReceiveOperation* ReceiveOperation(UObject* WorldContextObject, const FString& ObjectId,
							 TScriptInterface<ITransport> RemoteTransport, TScriptInterface<ITransport> LocalTransport);


	virtual void Activate() override;

	
protected:
	void Receive();
	
	FString ObjectId;
	TScriptInterface<ITransport> RemoteTransport;
	TScriptInterface<ITransport> LocalTransport;

	void HandleReceive(TSharedPtr<FJsonObject> Object);
	
	void HandleError(FString& Message);
};
