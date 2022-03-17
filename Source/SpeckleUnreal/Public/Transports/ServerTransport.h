// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Transport.h"

#include "ServerTransport.generated.h"


class FHttpModule;

/**
 *  Transport for receiving objects from a Speckle Server
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API UServerTransport : public UObject, public ITransport
{
 GENERATED_BODY()
	
protected:
	
	UPROPERTY()
	FString ServerUrl;
	UPROPERTY()
	FString StreamId;
	UPROPERTY(meta=(PasswordField))
	FString AuthToken;

	
	FTransportCopyObjectCompleteDelegate OnComplete;
	FTransportErrorDelegate OnError;
	
public:


	UFUNCTION(BlueprintPure, Category = "Speckle|Transports")
	static UServerTransport* CreateServerTransport(UPARAM(ref) FString& _ServerUrl, UPARAM(ref)  FString& _StreamId, UPARAM(ref) FString& _AuthToken)
	{
		UServerTransport* Transport = NewObject<UServerTransport>();
		Transport->ServerUrl = _ServerUrl;
		Transport->StreamId = _StreamId;
		Transport->AuthToken = _AuthToken;
		return Transport;
	}
	
	virtual TSharedPtr<FJsonObject> GetSpeckleObject(const FString& ObjectId) const override;
	virtual void SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject) override;
	
	virtual bool HasObject(const FString& ObjectId) const override;
	
 	virtual void CopyObjectAndChildren(const FString& ObjectId,
 		TScriptInterface<ITransport> TargetTransport,
 		const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
 		const FTransportErrorDelegate& OnErrorAction) override;

protected:
	static int32 SplitLines(const FString& Content, TArray<FString>& OutLines);
	
};
