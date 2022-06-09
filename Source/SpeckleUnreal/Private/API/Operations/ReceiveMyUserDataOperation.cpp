// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "API/Operations/ReceiveMyUserDataOperation.h"

#include "JsonObjectConverter.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


UReceiveMyUserDataOperation* UReceiveMyUserDataOperation::ReceiveMyUserDataOperation(
	UObject* WorldContextObject, const FString& ServerUrl, const FString& AuthToken)
{
    UReceiveMyUserDataOperation* Node = NewObject<UReceiveMyUserDataOperation>();
    Node->ServerUrl = ServerUrl.TrimEnd();;
	while(Node->ServerUrl.RemoveFromEnd("/")) { }
    Node->AuthToken = AuthToken.TrimEnd();;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UReceiveMyUserDataOperation::Activate()
{
	FAnalytics::TrackEvent(ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName()} });

	Request();
}


void UReceiveMyUserDataOperation::Request()
{
	const FString Query = TEXT("{\"query\": \"query{user{id name email company role suuid bio profiles avatar}}\"}");
	
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleError);
	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "user", Query,StaticClass()->GetName() , CompleteDelegate, ErrorDelegate);
}



void UReceiveMyUserDataOperation::HandleReceive(const FString& ResponseJson)
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
	OnReceiveSuccessfully.Broadcast(Response, "");		
	
	SetReadyToDestroy();
}

void UReceiveMyUserDataOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	const FSpeckleUser BlankUser{};
	OnError.Broadcast(BlankUser, Message);
	SetReadyToDestroy();
}
