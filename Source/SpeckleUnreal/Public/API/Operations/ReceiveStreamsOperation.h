// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/FSpeckleStream.h"

#include "ReceiveStreamsOperation.generated.h"

class ITransport;
class UBase;
class FJsonObject;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveStreamsOperationHandler, const TArray<FSpeckleStream>&, Streams, FString, ErrorMessage);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveStreamsOperationHandler, UBase*, RootBase, FString, ErrorMessage);

/**
 *	
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
	static UReceiveStreamsOperation* ReceiveStreamsOperation(UObject* WorldContextObject,
															 const FString& ObjectId,
															TScriptInterface<ITransport> RemoteTransport,
															TScriptInterface<ITransport> LocalTransport);
	virtual void Activate() override;
	
protected:
	void Receive();
	
	FString ObjectId;
	TScriptInterface<ITransport> RemoteTransport;
	TScriptInterface<ITransport> LocalTransport;

	void HandleReceive(TSharedPtr<FJsonObject> Object);
	
	void HandleError(FString& Message);
};
