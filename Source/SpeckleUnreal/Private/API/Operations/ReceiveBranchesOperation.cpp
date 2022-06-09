// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "API/Operations/ReceiveBranchesOperation.h"

#include "JsonObjectConverter.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"
#include "API/Models/SpeckleStream.h"


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
	FAnalytics::TrackEvent(ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName()} });
	
	Request();
}

void UReceiveBranchesOperation::Request()
{
	const FString Query = FString::Printf(TEXT("{\"query\": \"query{ stream (id: \\\"%s\\\"){id name branches(limit: %s) {totalCount cursor items{ id name description}}}}\"}"),
		*StreamId, *FString::FromInt(Limit));
	
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveBranchesOperation::HandleError);
	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "stream", Query,StaticClass()->GetName() , CompleteDelegate, ErrorDelegate);
}



void UReceiveBranchesOperation::HandleReceive(const FString& ResponseJson)
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
	OnReceiveSuccessfully.Broadcast(Response.Branches.Items, "");		
	
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

