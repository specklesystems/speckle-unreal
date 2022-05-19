// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveCustomOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Objects/Base.h"
#include "Mixpanel.h"


// ReceiveOperation
UReceiveCustomOperation* UReceiveCustomOperation::ReceiveCustomOperation(UObject* WorldContextObject,
													   const FString& ObjectId,
							                           TScriptInterface<ITransport> RemoteTransport,
							                           TScriptInterface<ITransport> LocalTransport)
{
    UReceiveCustomOperation* Node = NewObject<UReceiveCustomOperation>();
    Node->ObjectId = ObjectId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveCustomOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	Receive();
}

void UReceiveCustomOperation::Receive()
{
	check(LocalTransport != nullptr);
	
	// 1. Try and get object from local transport
	auto Obj = LocalTransport->GetSpeckleObject(ObjectId);

	if (Obj != nullptr )
	{
		HandleReceive(Obj);
		return;
	}

	// 2. Try and get object from remote transport
	if(RemoteTransport == nullptr)
	{
		FString ErrorMessage = TEXT(
			"Could not find specified object using the local transport, and you didn't provide a fallback remote from which to pull it.");

		HandleError(ErrorMessage);
		return;
	}

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveCustomOperation::HandleReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveCustomOperation::HandleError);
	
	RemoteTransport->CopyObjectAndChildren(ObjectId, LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveCustomOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
	
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		OnError.Broadcast(nullptr, FString::Printf(TEXT("Failed to get object %s from transport"), *ObjectId));
	}
	else
	{
		UBase* Res = USpeckleSerializer::DeserializeBase(Object, LocalTransport);
		if(IsValid(Res))
			OnReceiveOldSuccessfully.Broadcast(Res, "");
		else
			OnError.Broadcast(nullptr, FString::Printf(TEXT("Root Speckle Object %s failed to deserialize"), *ObjectId));
	}
		
	
	SetReadyToDestroy();
}

void UReceiveCustomOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	OnError.Broadcast(nullptr, Message);
	SetReadyToDestroy();
}