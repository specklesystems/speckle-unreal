// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveCommitsOperation.h"

#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


// ReceiveOperation
UReceiveCommitsOperation* UReceiveCommitsOperation::ReceiveCommitsOperation(UObject* WorldContextObject,
	const FString& ServerUrl, const FString& AuthToken,
	const FString& StreamId, const FString& BranchName, const int32 Limit)
{
    UReceiveCommitsOperation* Node = NewObject<UReceiveCommitsOperation>();
    Node->ServerUrl = ServerUrl.TrimEnd();
	while(Node->ServerUrl.RemoveFromEnd("/")) { }
	Node->AuthToken = AuthToken.TrimEnd();
    Node->StreamId = StreamId.TrimEnd();
	Node->BranchName = BranchName;
    Node->Limit = Limit;
	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveCommitsOperation::Activate()
{
	FAnalytics::TrackEvent("unknown", ServerUrl,
		 "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});
	
	Request();
}

void UReceiveCommitsOperation::Request()
{
	ensureAlways(Limit > 0);

	FFetchCommitDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleError);

	FClientAPI::StreamGetCommits(ServerUrl, AuthToken, StreamId, BranchName, Limit, CompleteDelegate, ErrorDelegate);
}

void UReceiveCommitsOperation::HandleReceive(const TArray<FSpeckleCommit>& Commits)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	FEditorScriptExecutionGuard ScriptGuard;
	
	OnReceiveSuccessfully.Broadcast(Commits, "");
	
	SetReadyToDestroy();
}

void UReceiveCommitsOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	
	const TArray<FSpeckleCommit> EmptyList;
	OnError.Broadcast(EmptyList, Message);
	
	SetReadyToDestroy();
}

