// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dom/JsonObject.h"
#include "Transport.generated.h"

class ITransport;

DECLARE_DELEGATE_OneParam(FTransportCopyObjectCompleteDelegate, TSharedPtr<FJsonObject>);
DECLARE_DELEGATE_OneParam(FTransportErrorDelegate, FString&);

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UTransport : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECKLEUNREAL_API ITransport
{
	GENERATED_BODY()

public:
 	virtual void SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject) = 0;

 	//virtual void SaveObjectFromTransport(FString& ObjectID, TScriptInterface<ITransport> SourceTransport) = 0;

 	virtual TSharedPtr<FJsonObject> GetSpeckleObject(const FString& ObjectId) const = 0;
	
 	virtual bool HasObject(const FString& ObjectId) const = 0;
	
 	virtual void CopyObjectAndChildren(const FString& ObjectId, TScriptInterface<ITransport> TargetTransport, const FTransportCopyObjectCompleteDelegate& OnCompleteAction, const FTransportErrorDelegate& OnErrorAction) = 0;

};
