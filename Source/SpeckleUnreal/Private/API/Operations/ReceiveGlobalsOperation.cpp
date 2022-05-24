// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveGlobalsOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"



UReceiveGlobalsOperation* UReceiveGlobalsOperation::ReceiveGlobalsOperation(
														UObject* WorldContextObject,
														const FString& ReferenceObjectId,
														TScriptInterface<ITransport> RemoteTransport,
														TScriptInterface<ITransport> LocalTransport)
{
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON START Globals with Ref Id %s"), *ReferenceObjectId);
	
	FString ObjectId = "Globals";
	
    UReceiveGlobalsOperation* Node = NewObject<UReceiveGlobalsOperation>();
    Node->ObjectId = ObjectId;
	Node->ReferenceObjectId = ReferenceObjectId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveGlobalsOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	ReceiveGlobals();
}

void UReceiveGlobalsOperation::ReceiveGlobals()
{
	check(LocalTransport != nullptr);
	
	// 1. Try and get object from local transport
	auto Obj = LocalTransport->GetSpeckleObject("Globals");

	//DisplayAsString("HandleGlobalsReceive", Obj);
	
	if (Obj != nullptr )
	{
		HandleGlobalsReceive(Obj);
		return;
	}

	// 2. Try and get object from remote transport
	if(RemoteTransport == nullptr)
	{
		FString ErrorMessage = TEXT(
			"Could not find specified object using the local transport, and you didn't provide a fallback remote from which to pull it.");

		HandleGlobalsError(ErrorMessage);
		return;
	}

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveGlobalsOperation::HandleGlobalsReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveGlobalsOperation::HandleGlobalsError);
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON RECEIVE 1 Globals"));
	
	RemoteTransport->CopyGlobals(this->ReferenceObjectId , LocalTransport, CompleteDelegate, ErrorDelegate);
}



void UReceiveGlobalsOperation::HandleGlobalsReceive(TSharedPtr<FJsonObject> Object)
{

	
	
	check(IsInGameThread())
		
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		const FSpeckleGlobals Globals;
		OnError.Broadcast(Globals, FString::Printf(TEXT("Speckle Globals object %s from transport"), *ObjectId));
	}
	else
	{
		const FSpeckleGlobals Globals = USpeckleSerializer::DeserializeGlobals(Object, LocalTransport);
		
		if(!Globals.ID.IsEmpty())
		{
			OnReceiveSuccessfully.Broadcast(Globals, "");
		}else
		{
			OnError.Broadcast(Globals,
				FString::Printf(TEXT("R5 Speckle Globals Object %s failed to deserialize"), *ObjectId));
		}
	}
	
	SetReadyToDestroy();
}

void UReceiveGlobalsOperation::HandleGlobalsError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	const FSpeckleGlobals Globals;
	OnError.Broadcast(Globals, Message);
	SetReadyToDestroy();
}

void UReceiveGlobalsOperation::DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj)
{
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
	UE_LOG(LogTemp, Log, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
}