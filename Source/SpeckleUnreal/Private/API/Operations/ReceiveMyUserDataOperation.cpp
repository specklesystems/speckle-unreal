// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveMyUserDataOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Objects/Base.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"


// ReceiveOperation
UReceiveMyUserDataOperation* UReceiveMyUserDataOperation::ReceiveMyUserDataOperation(

														UObject* WorldContextObject,
						
														TScriptInterface<ITransport> RemoteTransport,
														TScriptInterface<ITransport> LocalTransport)
{

	FString ObjectId = "MyUserData";

	
	
    UReceiveMyUserDataOperation* Node = NewObject<UReceiveMyUserDataOperation>();
    Node->ObjectId = ObjectId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveMyUserDataOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	Receive();
}

void UReceiveMyUserDataOperation::Receive()
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
	CompleteDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleError);
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON RECEIVE 1"));
	
	RemoteTransport->CopyMyUserData(LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveMyUserDataOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
		
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		FSpeckleUser MyUserData;
		OnError.Broadcast(MyUserData, FString::Printf(TEXT("Speckle user object %s from transport"), *ObjectId));
	}
	else
	{
		// --- Here Deserialize the list of Commits ----
		// It is not Deserializing the Commits well
		FSpeckleUser MyUserData = USpeckleSerializer::DeserializeMyUserData(Object, LocalTransport);
		
		if(!MyUserData.Id.IsEmpty())
		{
			OnReceiveSuccessfully.Broadcast(MyUserData, "");
		}else
		{
			OnError.Broadcast(MyUserData,
				FString::Printf(TEXT("R4 Speckle User Object %s failed to deserialize"), *ObjectId));
		}
	}
		
	
	SetReadyToDestroy();
}

void UReceiveMyUserDataOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	FSpeckleUser MyUserData;
	OnError.Broadcast(MyUserData, Message);
	SetReadyToDestroy();
}

