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
		UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON < CSTART <-----------"));
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






// My User Data
void UServerTransport::CopyMyUserData(
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;

	
	// Create Request for Root Object
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	// FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + StreamId +
	// 	"\\\"){ commits{items {id referencedObject sourceApplication totalChildrenCount branchName parents authorName authorId message createdAt commentCount}}}}\"}";
	
	FString PostPayload = "{\"query\": \"query{user{id,name,email,company,role,suuid,bio,profiles,avatar}}\"}"; 

	UE_LOG(LogSpeckle, Log, TEXT("-----------> PJSON MyUserData Payload: %s"), *PostPayload);
	
	Request->SetURL(ServerUrl + "/graphql");
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", TEXT("gzip"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetContentAsString(PostPayload);
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("MyUserData: Request for MyUserData %s returned %s. It was unsuccessful"),
													*Response->GetURL(), *Response->GetContentAsString());
			InvokeOnError(Message);
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("MyUserData: Request for MyUserData object with %s failed with HTTP response %d"),
													*Response->GetURL(), ResponseCode);
			InvokeOnError(Message);
		}

		UE_LOG(LogSpeckle, Log, TEXT("-----------> PJSON MyUserData: %s"), *Response->GetContentAsString());
		
		ResponseListOfCommitsSerialized = Response->GetContentAsString();
		
		TSharedPtr<FJsonObject> CommitsObj;
		if(!LoadJson(ResponseListOfCommitsSerialized, CommitsObj))
		{
			FString Message = FString::Printf(
				TEXT("Error R2: MyUserData Object %s was recieved but was invalid and could not be deserialied"),
											*ResponseListOfCommitsSerialized);
			InvokeOnError(Message);
			
		} else
		{
			UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON: Save Commit To %s"), TEXT("MyUserData"));
			
			TargetTransport->SaveObject("MyUserData", CommitsObj);

			ensureAlwaysMsgf(this->OnComplete.ExecuteIfBound(
														TargetTransport->GetSpeckleObject("MyUserData")),
														TEXT("Complete handler was not bound properly")
														);
		}
	};

	// Handle the response
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(
			TEXT("Request for Commits object at %s failed: \nHTTP request failed to start"),
					*PostPayload);
		
		InvokeOnError(Message);
		return;
	}
	UE_LOG(LogSpeckle, Verbose, TEXT("GET Request sent for Commits object at %s, awaiting response"), *PostPayload );
	FAnalytics::TrackEvent("unknown", ServerUrl, "Receive");
}








// Commits LIST
void UServerTransport::CopyListOfCommits(
										const FString& BranchName,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;

	
	// Create Request for Root Object
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	// FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + StreamId +
	// 	"\\\"){ commits{items {id referencedObject sourceApplication totalChildrenCount branchName parents authorName authorId message createdAt commentCount}}}}\"}";
	
	FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + StreamId +
		"\\\"){id name createdAt updatedAt branch(name: \\\"" + BranchName + "\\\" ){id name description author{name id email} " +
					"commits{totalCount items {id referencedObject sourceApplication totalChildrenCount " +
					"branchName parents authorName authorId authorAvatar message createdAt commentCount}}}}}\"}"; 

	UE_LOG(LogSpeckle, Log, TEXT("-----------> PJSON Commits Payload: %s"), *PostPayload);
	
	Request->SetURL(ServerUrl + "/graphql");
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", TEXT("gzip"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetContentAsString(PostPayload);
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Commits: Request for Branch %s returned %s. It was unsuccessful"),
													*Response->GetURL(), *Response->GetContentAsString());
			InvokeOnError(Message);
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("Branches: Request for Branch object with %s failed with HTTP response %d"),
													*Response->GetURL(), ResponseCode);
			InvokeOnError(Message);
		}

		UE_LOG(LogSpeckle, Log, TEXT("-----------> PJSON Commits: %s"), *Response->GetContentAsString());
		
		ResponseListOfCommitsSerialized = Response->GetContentAsString();
		
		TSharedPtr<FJsonObject> CommitsObj;
		if(!LoadJson(ResponseListOfCommitsSerialized, CommitsObj))
		{
			FString Message = FString::Printf(
				TEXT("Error R1: List of Commits Object %s was recieved but was invalid and could not be deserialied"),
											*ResponseListOfCommitsSerialized);
			InvokeOnError(Message);
			
		} else
		{
			UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON: Save Commit To %s"), *BranchName);
			
			TargetTransport->SaveObject(BranchName, CommitsObj);

			ensureAlwaysMsgf(this->OnComplete.ExecuteIfBound(
														TargetTransport->GetSpeckleObject(BranchName)),
														TEXT("Complete handler was not bound properly")
														);
		}
	};

	// Handle the response
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(
			TEXT("Request for Commits object at %s failed: \nHTTP request failed to start"),
					*PostPayload);
		
		InvokeOnError(Message);
		return;
	}
	UE_LOG(LogSpeckle, Verbose, TEXT("GET Request sent for Commits object at %s, awaiting response"), *PostPayload );
	FAnalytics::TrackEvent("unknown", ServerUrl, "Receive");
}





// Branches LIST
void UServerTransport::CopyListOfBranches(
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;
	
	// Create Request for Root Object
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();


	FString PostPayload = "{\"query\": \"query{\\n stream (id: \\\"" +
			StreamId + "\\\"){\\n id\\n name\\n branches {\\n totalCount\\n cursor\\n items{\\n id\\n name\\n description\\n}\\n }\\n }\\n}\"}";

	// The above can be extended with author information 
	//query{stream(id:"a18f8c8569"){id name branches{totalCount items{id name description author{id, name, email, commits{cursor}}}}}}
	
	Request->SetURL(ServerUrl + "/graphql");
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", TEXT("gzip"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetContentAsString(PostPayload);
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Branches: Request for root object at %s was unsuccessful: %s"),
													*Response->GetURL(), *Response->GetContentAsString());
			InvokeOnError(Message);
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("Branches: Request for root object at %s failed with HTTP response %d"),
													*Response->GetURL(), ResponseCode);
			InvokeOnError(Message);
		}

		UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON Branches: %s"), *Response->GetContentAsString());
		
		ResponseListOfBranchesSerialized = Response->GetContentAsString();
		
		TSharedPtr<FJsonObject> BranchesObj;
		if(!LoadJson(ResponseListOfBranchesSerialized, BranchesObj))
		{
			FString Message = FString::Printf(
				TEXT("A Speckle List of Branches Object %s was recieved but was invalid and could not be deserialied"),
											*ResponseListOfBranchesSerialized);
			InvokeOnError(Message);
			
		} else
		{
			UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON: Save To %s"), *StreamId);
			TargetTransport->SaveObject(StreamId, BranchesObj);

			ensureAlwaysMsgf(this->OnComplete.ExecuteIfBound(TargetTransport->GetSpeckleObject(StreamId)),
															   TEXT("Complete handler was not bound properly"));
		}
	};

	// Handle the response
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(
			TEXT("Request for Branches object at %s failed: \nHTTP request failed to start"),
					*PostPayload);
		
		InvokeOnError(Message);
		return;
	}
	UE_LOG(LogSpeckle, Verbose, TEXT("GET Request sent for Branches object at %s, awaiting response"), *PostPayload );
	FAnalytics::TrackEvent("unknown", ServerUrl, "Receive");
}





// STREAMS LIST
void UServerTransport::CopyListOfStreams(const FString& ObjectId,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction)
{
	this->OnComplete = OnCompleteAction;
	this->OnError = OnErrorAction;
	
	// Create Request for Root Object
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	// GraphQL: Here we have POST PAYLOAD but not endpoint
	const FString PostPayload =
		"{\"query\": \"query{user {streams(limit:50) {totalCount items {id name description updatedAt createdAt isPublic role}}}}\"}";
	
	Request->SetURL(ServerUrl + "/graphql");
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", TEXT("gzip"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetContentAsString(PostPayload);
	
	// Response Callback
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(!bWasSuccessful)
		{
			FString Message = FString::Printf(TEXT("Request for root object at %s was unsuccessful: %s"),
													*Response->GetURL(), *Response->GetContentAsString());
			InvokeOnError(Message);
		}
		
		const int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode != 200)
		{
			FString Message = FString::Printf(TEXT("Request for root object at %s failed with HTTP response %d"),
													*Response->GetURL(), ResponseCode);
			InvokeOnError(Message);
		}

		UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON: %s"), *Response->GetContentAsString());
		
		ResponseListOfStreamsSerialized = Response->GetContentAsString();
		
		TSharedPtr<FJsonObject> StreamsObj;
		if(!LoadJson(ResponseListOfStreamsSerialized, StreamsObj))
		{
			FString Message = FString::Printf(
				TEXT("A Speckle List of Streams Object %s was recieved but was invalid and could not be deserialied"),
											*ResponseListOfStreamsSerialized);
			InvokeOnError(Message);
			
		} else
		{
			UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON: Save To %s"), *ObjectId);
			TargetTransport->SaveObject(ObjectId, StreamsObj);

			ensureAlwaysMsgf(this->OnComplete.ExecuteIfBound(TargetTransport->GetSpeckleObject(ObjectId)),
															   TEXT("Complete handler was not bound properly"));
		}
	};

	// Handle the response
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	// Send request
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		FString Message = FString::Printf(
			TEXT("Request for Streams object at %s failed: \nHTTP request failed to start"),
					*PostPayload);
		
		InvokeOnError(Message);
		return;
	}
	UE_LOG(LogSpeckle, Verbose, TEXT("GET Request sent for Streams object at %s, awaiting response"), *PostPayload );
	FAnalytics::TrackEvent("unknown", ServerUrl, "Receive");
}


// Utilities

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

bool UServerTransport::LoadJson(const FString& StringJson, TSharedPtr<FJsonObject>& OutJsonObject)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StringJson);
	return FJsonSerializer::Deserialize(Reader, OutJsonObject);
}

void UServerTransport::InvokeOnError(FString& Message) const
{
	ensureAlwaysMsgf(this->OnError.ExecuteIfBound(Message), TEXT("ServerTransport: Unhandled error - %s"), *Message);
}