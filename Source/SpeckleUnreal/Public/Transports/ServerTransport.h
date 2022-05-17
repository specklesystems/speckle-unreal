// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Transport.h"

#include "ServerTransport.generated.h"


class FHttpModule;

// Data for graphQL request for object ids.
USTRUCT()
struct FObjectIdRequest
{
	GENERATED_BODY()
		
	UPROPERTY()
	TArray<FString> Ids;
};

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

	UPROPERTY()
	int32 MaxNumberOfObjectsPerRequest = 20000;

	
	FTransportCopyObjectCompleteDelegate OnComplete;
	FTransportErrorDelegate OnError;
	
public:


	UFUNCTION(BlueprintPure, Category = "Speckle|Transports")
	static UServerTransport* CreateServerTransport(UPARAM(ref) FString& _ServerUrl, UPARAM(ref)  FString& _StreamId,
																						UPARAM(ref) FString& _AuthToken)
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
	virtual void HandleRootObjectResponse(const FString& RootObjSerialized, TScriptInterface<ITransport> TargetTransport,
																			const FString& ObjectId) const;

    /**
	 * Iteratively fetches chunks of children
	 * @param TargetTransport the transport to store the fetched objects
	 * @param RootObjectId the id of the root object
	 * @param ChildrenIds array of all children to be fetched
	 * @param CStart the index in ChildrenIds of the start point of the current chunk
	 */
    virtual void FetchChildren(TScriptInterface<ITransport> TargetTransport,
	                            const FString& RootObjectId,
	                            const TArray<FString>& ChildrenIds,
	                            int32 CStart = 0) const;
	
	virtual void InvokeOnError(FString& Message) const;

	static bool LoadJson(const FString& ObjectJson, TSharedPtr<FJsonObject>& OutJsonObject);
	
	static int32 SplitLines(const FString& Content, TArray<FString>& OutLines);
};


