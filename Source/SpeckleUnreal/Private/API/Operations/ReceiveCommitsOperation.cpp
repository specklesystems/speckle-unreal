// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "API/Operations/ReceiveCommitsOperation.h"

#include "JsonObjectConverter.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"
#include "API/Models/SpeckleStream.h"


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
	FAnalytics::TrackEvent(ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName()} });

	
	Request();
}

void UReceiveCommitsOperation::Request()
{
	const FString Query = "{\"query\": \"query{stream (id: \\\"" + StreamId +
		"\\\"){id name createdAt updatedAt "
					+ "branch(name: \\\"" + BranchName + "\\\" ){id name description author{name id email} " +
					"commits(limit: " + FString::FromInt(Limit) + "){totalCount items {id referencedObject sourceApplication totalChildrenCount " +
					"branchName parents authorName authorId message createdAt commentCount}}}}}\"}"; // authorAvatar
	
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveCommitsOperation::HandleError);
	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "stream", Query,StaticClass()->GetName() , CompleteDelegate, ErrorDelegate);
}


void UReceiveCommitsOperation::HandleReceive(const FString& ResponseJson)
{
	check(IsInGameThread());
	
	FSpeckleStream Response;

	if(!FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseJson, &Response, 0, 0))
	{
		HandleError("Failed to deserialize object to SpeckleStreams");
		return;
	}
	
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Response.Branch.Commits.Items, "");		
	
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

