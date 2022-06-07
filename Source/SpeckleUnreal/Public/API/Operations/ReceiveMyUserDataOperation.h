// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Objects/HighLevel/SpeckleUser.h"

#include "ReceiveMyUserDataOperation.generated.h"

struct FSpeckleUser;

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
	

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations",
																meta = (WorldContext = "WorldContextObject"))
	static UReceiveMyUserDataOperation* ReceiveMyUserDataOperation(UObject* WorldContextObject,
		const FString& ServerUrl, const FString& AuthToken);
	
	virtual void Activate() override;
	
protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;

	void HandleReceive(const FSpeckleUser& Object);
	
	void HandleError(const FString& Message);
};
