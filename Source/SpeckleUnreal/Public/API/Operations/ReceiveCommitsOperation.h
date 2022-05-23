// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleCommit.h"


#include "ReceiveCommitsOperation.generated.h"

class ITransport;
class UBase;
class FJsonObject;


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

	
	
	
	/**
	 * @brief 
	 * @param WorldContextObject    
	 * @param BranchName : Give the Branch Name, e.g. main
	 * @param RemoteTransport  
	 * @param LocalTransport 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations",
						meta = (WorldContext = "WorldContextObject", BranchName = "main"))
	static UReceiveCommitsOperation* ReceiveCommitsOperation(UObject* WorldContextObject,
	                                                         const FString& BranchName,
															 TScriptInterface<ITransport> RemoteTransport,
															 TScriptInterface<ITransport> LocalTransport);
	virtual void Activate() override;
	
protected:
	void Receive();
	
	FString ObjectId;
	FString BranchName;
	
	FString StreamId;
	

	
	TScriptInterface<ITransport> RemoteTransport;
	TScriptInterface<ITransport> LocalTransport;

	void HandleReceive(TSharedPtr<FJsonObject> Object);
	
	void HandleError(FString& Message);
};
