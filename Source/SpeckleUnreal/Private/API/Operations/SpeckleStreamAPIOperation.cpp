// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "API/Operations/SpeckleStreamAPIOperation.h"

#include "JsonObjectConverter.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


USpeckleStreamAPIOperation* USpeckleStreamAPIOperation::SpeckleStreamAPIOperation(
																		const FString& ServerUrl,
																		const FString& AuthToken,
																		const FString& GraphQlQuery,
																		const FString& ResponsePropertyName, 
																		const FString& RequestLogName)
{
    USpeckleStreamAPIOperation* Node = NewObject<USpeckleStreamAPIOperation>();
    Node->ServerUrl = ServerUrl.TrimEnd();
	Node->AuthToken = AuthToken.TrimEnd();
	Node->Query = GraphQlQuery;
	Node->ResponsePropertyName = ResponsePropertyName;
	Node->RequestLogName = RequestLogName;
	
    return Node;
}

void USpeckleStreamAPIOperation::Activate()
{
	FAnalytics::TrackEvent(ServerUrl, "NodeRun", TMap<FString, FString> { {"name", RequestLogName} });

	Request();
}

void USpeckleStreamAPIOperation::Request()
{
	FString Payload = FString::Printf(TEXT("{\"query\": \"%s\"}"), *Query.ReplaceCharWithEscapedChar());
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &USpeckleStreamAPIOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &USpeckleStreamAPIOperation::HandleError);
	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, ResponsePropertyName, Payload,RequestLogName , CompleteDelegate, ErrorDelegate);
}

void USpeckleStreamAPIOperation::HandleReceive(const FString& ResponseJson)
{
	check(IsInGameThread());
	
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(ResponseJson, "");		
	
	SetReadyToDestroy();
}

void USpeckleStreamAPIOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnError.Broadcast("", Message);
		
	SetReadyToDestroy();
}

