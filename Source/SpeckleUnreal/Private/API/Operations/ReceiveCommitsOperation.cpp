// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveCommitsOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Objects/Base.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"


// ReceiveOperation
UReceiveCommitsOperation* UReceiveCommitsOperation::ReceiveCommitsOperation(

														UObject* WorldContextObject,
														const FString& BranchName,
														TScriptInterface<ITransport> RemoteTransport,
														TScriptInterface<ITransport> LocalTransport)
{

	FString ObjectId = "Commits";

	
	
    UReceiveCommitsOperation* Node = NewObject<UReceiveCommitsOperation>();
    Node->ObjectId = ObjectId;
	Node->BranchName = BranchName;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveCommitsOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	Receive();
}

void UReceiveCommitsOperation::Receive()
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
	CompleteDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleError);
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON RECEIVE 1"));
	
	RemoteTransport->CopyListOfCommits( BranchName, LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveCommitsOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
		
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		TArray<FSpeckleCommit> EmptyListOfCommits;
		OnError.Broadcast(EmptyListOfCommits, FString::Printf(TEXT("Speckle list of commits object %s from transport"), *ObjectId));
	}
	else
	{
		// --- Here Deserialize the list of Commits ----
		// It is not Deserializing the Commits well
		TArray<FSpeckleCommit> FullListOfCommits = USpeckleSerializer::DeserializeListOfCommits(Object, LocalTransport);
		
		if(FullListOfCommits.Num()>0)
		{
			OnReceiveSuccessfully.Broadcast(FullListOfCommits, "");
		}else
		{
			TArray<FSpeckleCommit> EmptyListOfCommits;
			OnError.Broadcast(EmptyListOfCommits,
				FString::Printf(TEXT("Speckle list of commits Object %s failed to deserialize"), *ObjectId));
		}
	}
		
	
	SetReadyToDestroy();
}

void UReceiveCommitsOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	TArray<FSpeckleCommit> EmptyListOfCommits;
	OnError.Broadcast(EmptyListOfCommits, Message);
	SetReadyToDestroy();
}

