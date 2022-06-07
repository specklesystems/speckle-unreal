// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveGlobalsOperation.h"

#include "Mixpanel.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"


UReceiveGlobalsOperation* UReceiveGlobalsOperation::ReceiveGlobalsOperation(UObject* WorldContextObject,
                                                                            const FString& ServerUrl, const FString& AuthToken,
                                                                            const FString& StreamId, const FString& ReferencedObjectId)
{
    UReceiveGlobalsOperation* Node = NewObject<UReceiveGlobalsOperation>();
    Node->ServerUrl = ServerUrl.TrimEnd();
	while(Node->ServerUrl.RemoveFromEnd("/")) { }
	Node->AuthToken = AuthToken.TrimEnd();;
    Node->StreamId = StreamId.TrimEnd();;
    Node->ReferencedObjectId = ReferencedObjectId;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UReceiveGlobalsOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});
	
	Request();
}

void UReceiveGlobalsOperation::Request()
{
	FFetchGlobalsDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveGlobalsOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveGlobalsOperation::HandleError);
	
	FClientAPI::FetchGlobals(ServerUrl, AuthToken, StreamId, ReferencedObjectId, CompleteDelegate, ErrorDelegate);
}



void UReceiveGlobalsOperation::HandleReceive(const FSpeckleGlobals& Object)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Object, "");		
	
	SetReadyToDestroy();
}

void UReceiveGlobalsOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	const FSpeckleGlobals BlankGlobals;
	OnError.Broadcast(BlankGlobals, Message);
	SetReadyToDestroy();
}
