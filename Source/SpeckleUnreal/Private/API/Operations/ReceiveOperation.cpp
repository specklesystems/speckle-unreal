// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveOperation.h"

#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Objects/Base.h"
#include "LogSpeckle.h"


UReceiveOperation* UReceiveOperation::ReceiveOperation(UObject* WorldContextObject, const FString& ObjectId, TScriptInterface<ITransport> RemoteTransport, TScriptInterface<ITransport> LocalTransport)
{
    UReceiveOperation* Node = NewObject<UReceiveOperation>();
    Node->ObjectId = ObjectId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}


void UReceiveOperation::Activate()
{
	//Async(EAsyncExecution::Thread, [this]{Receive();});
	
	Receive();
}

void UReceiveOperation::Receive()
{
	check(LocalTransport != nullptr);
	
	// 1. Try and get object from local transport
	auto Obj = LocalTransport->GetSpeckleObject(ObjectId);

	if (Obj != nullptr )
	{
		HandleReceive(Obj);
	}

	// 2. Try and get object from remote transport
	if(RemoteTransport == nullptr)
	{
		FString ErrorMessage = TEXT("Could not find specified object using the local transport, and you didn't provide a fallback remote from which to pull it.");
		HandleError(ErrorMessage);
		return;
	}

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveOperation::HandleReceive);
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveOperation::HandleError);
	
	RemoteTransport->CopyObjectAndChildren(ObjectId, LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
	
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		OnError.Broadcast(nullptr, FString::Printf(TEXT("Failed to get object %s from transport"), *ObjectId));
	}
	else
	{
		UBase* Res = FSpeckleSerializer::DeserializeBase(Object, LocalTransport);
		if(IsValid(Res))
			OnReceiveSuccessfully.Broadcast(Res, "");
		else
			OnError.Broadcast(nullptr, FString::Printf(TEXT("Root Speckle Object %s failed to deserialize"), *ObjectId));
	}
		
	
	SetReadyToDestroy();
}

void UReceiveOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	OnError.Broadcast(nullptr, Message);
	SetReadyToDestroy();
}

