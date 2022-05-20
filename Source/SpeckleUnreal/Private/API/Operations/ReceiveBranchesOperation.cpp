// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveBranchesOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"


// ReceiveOperation
UReceiveBranchesOperation* UReceiveBranchesOperation::ReceiveBranchesOperation(UObject* WorldContextObject,
														const FString& StreamId,
							                           TScriptInterface<ITransport> RemoteTransport,
							                           TScriptInterface<ITransport> LocalTransport)
{
	FString ObjectId = "Streams";
	
    UReceiveBranchesOperation* Node = NewObject<UReceiveBranchesOperation>();
    Node->StreamId = StreamId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;
	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveBranchesOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	Receive();
}

void UReceiveBranchesOperation::Receive()
{
	check(LocalTransport != nullptr);
	
	// 1. Try and get object from local transport
	auto Obj = LocalTransport->GetSpeckleObject(StreamId);

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
	CompleteDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleError);
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON RECEIVE 1"));
	
	RemoteTransport->CopyListOfBranches(LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveBranchesOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
		
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		TArray<FSpeckleBranch> EmptyListOfBranches;
		OnError.Broadcast(EmptyListOfBranches, FString::Printf(TEXT("Failed to get Branches from Stream: %s"), *StreamId));
	}
	else
	{
		// --- Here Deserialize the list of Streams ----
		// It is not Deserializing the Streams well
		TArray<FSpeckleBranch> FullListOfBranches = USpeckleSerializer::DeserializeListOfBranches(Object, LocalTransport);

		
		if(FullListOfBranches.Num()>0)
		{
			OnReceiveSuccessfully.Broadcast(FullListOfBranches, "");
		}else
		{
			TArray<FSpeckleBranch> EmptyListOfBranches;
			OnError.Broadcast(EmptyListOfBranches,
				FString::Printf(TEXT("Root Speckle Stream Object %s failed to deserialize"), *StreamId));
		}
	}
		
	
	SetReadyToDestroy();
}

void UReceiveBranchesOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	TArray<FSpeckleBranch> EmptyListOfBranches;
	OnError.Broadcast(EmptyListOfBranches, Message);
	SetReadyToDestroy();
}

