// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleGlobals.h"


#include "ReceiveGlobalsOperation.generated.h"

class ITransport;
class UBase;
class FJsonObject;


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
	
	/**
	 * @brief 
	 * @param WorldContextObject
	 * @param ReferenceObjectId : The id of the Speckle Object to fetch     
	 * @param RemoteTransport  
	 * @param LocalTransport 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations",
						meta = (WorldContext = "WorldContextObject"))
	static UReceiveGlobalsOperation* ReceiveGlobalsOperation(UObject* WorldContextObject,
															 const FString& ReferenceObjectId,
															 TScriptInterface<ITransport> RemoteTransport,
															 TScriptInterface<ITransport> LocalTransport);
	virtual void Activate() override;

	static void DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj);
protected:
	void ReceiveGlobals();
	
	FString ObjectId;
	FString ReferenceObjectId;
	
	TScriptInterface<ITransport> RemoteTransport;
	TScriptInterface<ITransport> LocalTransport;

	void HandleGlobalsReceive(TSharedPtr<FJsonObject> Object);
	
	void HandleGlobalsError(FString& Message);
};
