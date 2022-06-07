// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveStreamsOperation.h"

#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


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
	FAnalytics::TrackEvent("unknown",
		ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	Request();
}

void UReceiveStreamsOperation::Request()
{
	FFetchStreamDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleError);

	FClientAPI::StreamsGet(ServerUrl, AuthToken, Limit, CompleteDelegate, ErrorDelegate);
}

void UReceiveStreamsOperation::HandleReceive(const TArray<FSpeckleStream>& Streams)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Streams, "");		
	
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

