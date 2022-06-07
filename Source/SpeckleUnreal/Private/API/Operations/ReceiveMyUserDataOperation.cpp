// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveMyUserDataOperation.h"

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
	FAnalytics::TrackEvent("unknown",
		ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	Request();
}


void UReceiveMyUserDataOperation::Request()
{
	FFetchUserDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleReceive);

	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveMyUserDataOperation::HandleError);
	
	FClientAPI::FetchUserData(ServerUrl, AuthToken, CompleteDelegate, ErrorDelegate);
}



void UReceiveMyUserDataOperation::HandleReceive(const FSpeckleUser& Object)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Log, TEXT("%s to %s Succeeded"), *StaticClass()->GetName(), *ServerUrl);
	FEditorScriptExecutionGuard ScriptGuard;
	OnReceiveSuccessfully.Broadcast(Object, "");		
	
	SetReadyToDestroy();
}

void UReceiveMyUserDataOperation::HandleError(const FString& Message)
{
	check(IsInGameThread());
	UE_LOG(LogSpeckle, Warning, TEXT("%s failed - %s"), *StaticClass()->GetName(), *Message);
	
	FEditorScriptExecutionGuard ScriptGuard;
	const FSpeckleUser BlankUser;
	OnError.Broadcast(BlankUser, Message);
	SetReadyToDestroy();
}
