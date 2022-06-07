// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveBranchesOperation.h"

#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


UReceiveBranchesOperation* UReceiveBranchesOperation::ReceiveBranchesOperation(UObject* WorldContextObject,
                                                                               const FString& ServerUrl, const FString& AuthToken,
                                                                               const FString& StreamId, int32 Limit)
{
	FString ObjectId = "Streams";
	
    UReceiveBranchesOperation* Node = NewObject<UReceiveBranchesOperation>();
    Node->StreamId = StreamId.TrimEnd();
    Node->ServerUrl = ServerUrl.TrimEnd();
	while(Node->ServerUrl.RemoveFromEnd("/")) { }
	Node->AuthToken = AuthToken.TrimEnd();
	Node->Limit = Limit;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UReceiveBranchesOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});
	
	Request();
}

void UReceiveBranchesOperation::Request()
{
	ensureAlways(Limit > 0);

	FFetchBranchDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleReceive);
	
	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleError);

	FClientAPI::StreamGetBranches(ServerUrl, AuthToken, StreamId, Limit, CompleteDelegate, ErrorDelegate);
}

void UReceiveBranchesOperation::HandleReceive(const TArray<FSpeckleBranch>& Branches)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Branches, "");		
	
	SetReadyToDestroy();
}

void UReceiveBranchesOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);

	FEditorScriptExecutionGuard ScriptGuard;

	const TArray<FSpeckleBranch> EmptyList;
	OnError.Broadcast(EmptyList, Message);
	
	SetReadyToDestroy();
}

