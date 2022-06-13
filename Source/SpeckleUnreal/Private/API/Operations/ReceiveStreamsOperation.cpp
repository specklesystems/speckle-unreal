// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "API/Operations/ReceiveStreamsOperation.h"

#include "JsonObjectConverter.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"
#include "API/Models/SpeckleUser.h"


UReceiveStreamsOperation* UReceiveStreamsOperation::ReceiveStreamsOperation(UObject* WorldContextObject,
                                                                            const FString& ServerUrl,
                                                                            const FString& AuthToken,
                                                                            const int32 Limit)
{
	FString ObjectId = "Streams";
	
    UReceiveStreamsOperation* Node = NewObject<UReceiveStreamsOperation>();
    Node->ServerUrl = ServerUrl.TrimEnd();;
	Node->AuthToken = AuthToken.TrimEnd();;
	Node->Limit = Limit;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UReceiveStreamsOperation::Activate()
{
	FAnalytics::TrackEvent(ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName()} });


	Request();
}

void UReceiveStreamsOperation::Request()
{
	const FString Query = FString::Printf(TEXT("{\"query\": \"query{user {streams(limit:%s) {totalCount items {id name description updatedAt createdAt isPublic role  collaborators{id name role company avatar}}}}}\"}"),
		*FString::FromInt(Limit));
	
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleError);
	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "user", Query,StaticClass()->GetName() , CompleteDelegate, ErrorDelegate);
}

void UReceiveStreamsOperation::HandleReceive(const FString& ResponseJson)
{
	check(IsInGameThread());
	
	FSpeckleUser Response;

	 if(!FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseJson, &Response, 0, 0))
	 {
	 	HandleError("Failed to deserialize object to SpeckleStreams");
	 	return;
	 }
	
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Response.Streams.Items, "");		
	
	SetReadyToDestroy();
}

void UReceiveStreamsOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	
	const TArray<FSpeckleStream> EmptyList;
	OnError.Broadcast(EmptyList, Message);
		
	SetReadyToDestroy();
}

