// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleUser.h"


#include "ReceiveMyUserDataOperation.generated.h"

class ITransport;
class UBase;
class FJsonObject;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceiveMyUserDataOperationHandler, const FSpeckleUser, MyUserData, FString, ErrorMessage);


/**
 *   Receive My User Data in Speckle
 */
UCLASS()
class SPECKLEUNREAL_API UReceiveMyUserDataOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
    UPROPERTY(BlueprintAssignable)
    FReceiveMyUserDataOperationHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FReceiveMyUserDataOperationHandler OnError;
	
	/**
	 * @brief 
	 * @param WorldContextObject    
	 * @param RemoteTransport  
	 * @param LocalTransport 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations",
																meta = (WorldContext = "WorldContextObject"))
	static UReceiveMyUserDataOperation* ReceiveMyUserDataOperation(UObject* WorldContextObject,
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
