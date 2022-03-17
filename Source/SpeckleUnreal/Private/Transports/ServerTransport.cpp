// Fill out your copyright notice in the Description page of Project Settings.


#include "Transports/ServerTransport.h"

#include <LogSpeckle.h>

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"



TSharedPtr<FJsonObject> UServerTransport::GetSpeckleObject(const FString& ObjectId) const
{
	unimplemented();
	return nullptr;
}

void UServerTransport::SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject)
{
	unimplemented(); //TODO implement
}

bool UServerTransport::HasObject(const FString& ObjectId) const
{
	unimplemented(); //TODO implement
	return false;
}

void UServerTransport::CopyObjectAndChildren(const FString& ObjectId,
											 TScriptInterface<ITransport> TargetTransport,
											 const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
											 const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;
	
	const FString Endpoint = ServerUrl + "/objects/" + StreamId + "/" + ObjectId;
	const FString UserAgent = FString::Printf(TEXT("Unreal Engine (%s) / %d.%d.%d"), *UGameplayStatics::GetPlatformName(), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);
	
	// Create Request
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(Endpoint);
	Request->SetHeader("Accept", TEXT("text/plain"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetHeader("User-Agent", UserAgent);
	
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Stream Request failed: %s"), *Response->GetContentAsString());
			this->OnError.Execute(Message);
			return;
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("Can't get object %s/%s: HTTP error %d"), *StreamId, *ObjectId, ResponseCode);
			this->OnError.Execute(Message);
			return;
		}

		FString Content = Response->GetContentAsString();

		TArray<FString> Lines;
		const int32 LineCount = SplitLines(Content, Lines);
		
		UE_LOG(LogSpeckle, Log, TEXT("Parsing %d downloaded objects..."), LineCount)
	
		for (const FString& Line : Lines)
		{
			FString Id, ObjectJson;
			if (!Line.Split("\t", &Id, &ObjectJson))
				continue;
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ObjectJson);
			if (!FJsonSerializer::Deserialize(Reader, JsonObject))
				continue;

			TargetTransport->SaveObject(Id, JsonObject);
		}

		this->OnComplete.Execute(TargetTransport->GetSpeckleObject(ObjectId));

	};
	
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(TEXT("Can't get object %s/%s: HTTP request failed to start"), *StreamId, *ObjectId);
		OnErrorAction.Execute(Message);
	}
}

int32 UServerTransport::SplitLines(const FString& Content, TArray<FString>& OutLines)
{
	int32 LineCount = 0;
	for (const TCHAR* ptr = *Content; *ptr; ptr++)
		if (*ptr == '\n')
			LineCount++;
	OutLines.Reserve(LineCount);
	Content.ParseIntoArray(OutLines, TEXT("\n"), true);
	return LineCount;
}
