// Fill out your copyright notice in the Description page of Project Settings.


#include "Transports/ServerTransport.h"
#include "LogSpeckle.h"
#include "Mixpanel.h"

#include "JsonObjectConverter.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Policies/CondensedJsonPrintPolicy.h"


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

// Create HTTP Request for Commit Root objects (only ids of children)
void UServerTransport::CopyObjectAndChildren(const FString& ObjectId,
											 TScriptInterface<ITransport> TargetTransport,
											 const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
											 const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;
	
	// Create Request for Root Object
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	const FString Endpoint = FString::Printf(TEXT("%s/objects/%s/%s/single"), *ServerUrl, *StreamId, *ObjectId);
	Request->SetVerb("GET");
	Request->SetURL(Endpoint);
	Request->SetHeader("Accept", TEXT("text/plain"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
		
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Request for root object at %s was unsuccessful: %s"), *Response->GetURL(), *Response->GetContentAsString());
			InvokeOnError(Message);
			return;
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("Request for root object at %s failed with HTTP response %d"), *Response->GetURL(), ResponseCode);
			InvokeOnError(Message);
			return;
		}

		HandleRootObjectResponse(Response->GetContentAsString(), TargetTransport, ObjectId);
	};
	
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(TEXT("Request for root object at %s failed: \nHTTP request failed to start"), *Endpoint);
		InvokeOnError(Message);
		return;
	}
	UE_LOG(LogSpeckle, Verbose, TEXT("GET Request sent for root object at %s, awaiting response"), *Endpoint );
	FAnalytics::TrackEvent("unknown", ServerUrl, "Receive");
}



// Fetch Children JSON and parse it
void UServerTransport::FetchChildren(TScriptInterface<ITransport> TargetTransport, const FString& ObjectId,
																const TArray<FString>& ChildrenIds, int32 CStart) const
{
	// Check if all children have been fetched
	if(ChildrenIds.Num() <= CStart)
	{
		ensureAlwaysMsgf(this->OnComplete.ExecuteIfBound(TargetTransport->GetSpeckleObject(ObjectId)),
		                                                               TEXT("Complete handler was not bound properly"));
		return;
	}
	
	// Assemble list of ids to ask for in this request
	// We want to avoid making requests too large
	const int32 CEnd = FGenericPlatformMath::Min(ChildrenIds.Num(), CStart + MaxNumberOfObjectsPerRequest);
	FString ChildrenIdsString;
	{
		auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&ChildrenIdsString);
		Writer->WriteArrayStart();
		for (int32 i = CStart; i < CEnd; i++)
		{
			Writer->WriteValue(ChildrenIds[i]);
		}
		Writer->WriteArrayEnd();
		Writer->Close();
	}

	FString Body;
	{
		auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Body);
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("objects"), ChildrenIdsString);
		Writer->WriteObjectEnd();
		Writer->Close();
	}
	
	// Create Request
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	{
		const FString EndPoint = FString::Printf(TEXT("%s/api/getobjects/%s"), *ServerUrl, *StreamId);
		Request->SetVerb("POST");
		Request->SetURL(EndPoint);
		Request->SetHeader("Accept", TEXT("text/plain"));
		Request->SetHeader("Authorization", "Bearer " + AuthToken);
		Request->SetHeader("Content-Type", "application/json");
		Request->SetContentAsString(Body);
	}
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		// Any Fail
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Request for children of root object %s/%s failed: %s"),
																*StreamId,  *ObjectId, *Response->GetContentAsString());
			InvokeOnError(Message);
			return;
		}

		// Any HTTP Fail
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(
								TEXT("Request for children of root object %s/%s failed:\nHTTP response %d"),
								                                               *StreamId,  *ObjectId, ResponseCode);
			InvokeOnError(Message);
			return;
		}

		// Success: Start parsing
		TArray<FString> Lines;
		const int32 LineCount = SplitLines(Response->GetContentAsString(), Lines);
		
		UE_LOG(LogSpeckle, Verbose, TEXT("Parsing %d downloaded objects..."), LineCount)

		// Warning: Less objects then expected
		if(LineCount != CEnd - CStart)
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Requested %d objects, but received %d"), CEnd - CStart, LineCount);
		}

		// Load JSON as objects
		for (const FString& Line : Lines)
		{
			FString Id, ObjectJson;
			if (!Line.Split("\t", &Id, &ObjectJson))
				continue;
			
			TSharedPtr<FJsonObject> JsonObject;
			if(!LoadJson(ObjectJson, JsonObject)) continue;

			TargetTransport->SaveObject(Id, JsonObject);
		}

		UE_LOG(LogSpeckle, Log, TEXT("Processed %d/%d Child objects"), CEnd, ChildrenIds.Num())
		
		//Iterate again for any missing children
		FetchChildren(TargetTransport, ObjectId, ChildrenIds, CEnd);
	};
	
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request for children
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(TEXT("Failed to fetch children of root object %s/%s:\nHTTP request failed to start"), *StreamId, *ObjectId);
		InvokeOnError(Message);
		return;
	}
	
	UE_LOG(LogSpeckle, Verbose, TEXT("Requesting %d child objects"), CEnd - CStart);
}

// Parse Root JSON
void UServerTransport::HandleRootObjectResponse(const FString& RootObjSerialized,
												TScriptInterface<ITransport> TargetTransport,
												const FString& ObjectId) const
{
	TSharedPtr<FJsonObject> RootObj;
	if(!LoadJson(RootObjSerialized, RootObj))
	{
		FString Message = FString::Printf( TEXT("A Root Object %s was recieved but was invalid and could not be deserialied"), *ObjectId);
		InvokeOnError(Message);
		return;
	}
	
	TargetTransport->SaveObject(ObjectId, RootObj);	
	
	// Find children are not already in the target transport
	const auto Closures = RootObj->GetObjectField("__closure")->Values;
	
	TArray<FString> ChildrenIds; 
	Closures.GetKeys(ChildrenIds);
	TArray<FString> NewChildrenIds;
	for(const FString& Id : ChildrenIds)
	{
		if(TargetTransport->HasObject(Id)) continue;
		
		NewChildrenIds.Add(Id);
	}
	
	FetchChildren(TargetTransport, ObjectId, NewChildrenIds);	
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

bool UServerTransport::LoadJson(const FString& ObjectJson, TSharedPtr<FJsonObject>& OutJsonObject)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ObjectJson);
	return FJsonSerializer::Deserialize(Reader, OutJsonObject);
}

void UServerTransport::InvokeOnError(FString& Message) const
{
	ensureAlwaysMsgf(this->OnError.ExecuteIfBound(Message), TEXT("ServerTransport: Unhandled error - %s"), *Message);
}

