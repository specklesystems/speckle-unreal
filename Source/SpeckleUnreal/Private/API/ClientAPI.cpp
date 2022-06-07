#include "API/ClientAPI.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "LogSpeckle.h"
#include "Mixpanel.h"
#include "Objects/HighLevel/SpeckleBranch.h"
#include "Objects/HighLevel/SpeckleCommit.h"
#include "Objects/HighLevel/SpeckleGlobals.h"
#include "Objects/HighLevel/SpeckleUser.h"
#include "Transports/Transport.h"


void FClientAPI::StreamsGet(const FString& ServerUrl, const FString& AuthToken,
	const int Limit,
	const FFetchStreamDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	const FString RequestLogName = FString(__FUNCTION__);
	
	const FString PostPayload =
		"{\"query\": \"query{user {streams(limit:" + FString::FromInt(Limit) + ") {totalCount items {id name description updatedAt createdAt isPublic role  collaborators{id name role company avatar}}}}}\"}";
	
	auto OnError = [=](const FString& Message) mutable 
	{
		ensureAlwaysMsgf(OnErrorAction.ExecuteIfBound(Message), TEXT("%s: Unhandled error - %s"), *RequestLogName , *Message);
	};
	
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(CheckRequestFailed(bWasSuccessful, Response, RequestLogName, OnError)) return;
		
		TSharedPtr<FJsonObject> Obj;
		if(!GetResponseAsJSON(Response, RequestLogName, Obj, OnError)) return;
	
		TArray<TSharedPtr<FJsonValue>> StreamsArrJSON = Obj->GetObjectField(TEXT("data"))
																   ->GetObjectField(TEXT("user"))
																	  ->GetObjectField(TEXT("streams"))
																		   ->GetArrayField(TEXT("items"));
	
		TArray<FSpeckleStream> Streams;
		Streams.Reserve(StreamsArrJSON.Num());
	
		for (const TSharedPtr<FJsonValue> v : StreamsArrJSON)
		{
			FSpeckleStream Stream = FSpeckleStream(v);
			Streams.Add(Stream);
		}
		
		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully. Fetched %d items"), *RequestLogName, Streams.Num());
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(Streams), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};
	
	
	const FHttpRequestRef Request = CreateGraphQLRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	SendGraphQLRequest(Request, RequestLogName, OnError);
}

void FClientAPI::StreamGetBranches(
	const FString& ServerUrl, const FString& AuthToken,
	const FString& StreamId, const int32 Limit,
	const FFetchBranchDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	const FString RequestLogName = FString(__FUNCTION__);
	
	//TODO limit
	FString PostPayload = "{\"query\": \"query{ stream (id: \\\"" +
			StreamId + "\\\"){id name branches(limit : " + FString::FromInt(Limit) + ") {totalCount cursor items{ id name description}}}}\"}";

	// The above can be extended with author information 
	//query{stream(id:"a18f8c8569"){id name branches{totalCount items{id name description author{id, name, email, commits{cursor}}}}}}
	
	auto OnError = [=](const FString& Message) mutable 
	{
		ensureAlwaysMsgf(OnErrorAction.ExecuteIfBound(Message), TEXT("%s: Unhandled error - %s"), *RequestLogName , *Message);
	};
	
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(CheckRequestFailed(bWasSuccessful, Response, RequestLogName, OnError)) return;
		
		TSharedPtr<FJsonObject> Obj;
		if(!GetResponseAsJSON(Response, RequestLogName, Obj, OnError)) return;
		
		TArray<TSharedPtr<FJsonValue>> BranchesArrJSON = Obj->GetObjectField(TEXT("data"))
																	->GetObjectField(TEXT("stream"))
																		->GetObjectField(TEXT("branches"))
																				->GetArrayField(TEXT("items"));
		TArray<FSpeckleBranch> Branches;
		Branches.Reserve(BranchesArrJSON.Num());
	
		for (const TSharedPtr<FJsonValue>& v : BranchesArrJSON)
		{
			const TSharedPtr<FJsonObject>* BranchObj;
			if(ensureAlways(v->TryGetObject(BranchObj)))
			{
				FSpeckleBranch Branch = FSpeckleBranch(*BranchObj);
				Branches.Add(Branch);
			}
		}
		
		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully. Fetched %d items"), *RequestLogName, Branches.Num());
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(Branches), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};
	
	
	const FHttpRequestRef Request = CreateGraphQLRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	SendGraphQLRequest(Request, RequestLogName, OnError);
}


void FClientAPI::StreamGetCommits(const FString& ServerUrl, const FString& AuthToken,
	const FString& StreamId, const FString& BranchName, const int32 Limit,
	const FFetchCommitDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	const FString RequestLogName = FString(__FUNCTION__);
	
	FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + StreamId +
		"\\\"){id name createdAt updatedAt "
					+ "branch(name: \\\"" + BranchName + "\\\" ){id name description author{name id email} " +
					"commits(limit: " + FString::FromInt(Limit) + "){totalCount items {id referencedObject sourceApplication totalChildrenCount " +
					"branchName parents authorName authorId message createdAt commentCount}}}}}\"}"; // authorAvatar 
	
	auto OnError = [=](const FString& Message) mutable 
	{
		ensureAlwaysMsgf(OnErrorAction.ExecuteIfBound(Message), TEXT("%s: Unhandled error - %s"), *RequestLogName , *Message);
	};
	
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(CheckRequestFailed(bWasSuccessful, Response, RequestLogName, OnError)) return;
		
		TSharedPtr<FJsonObject> Obj;
		if(!GetResponseAsJSON(Response, RequestLogName, Obj, OnError)) return;
		
		TArray<TSharedPtr<FJsonValue>> CommitsArrJSON = Obj->GetObjectField(TEXT("data"))
															 ->GetObjectField(TEXT("stream"))
															   ->GetObjectField(TEXT("branch"))
																 ->GetObjectField(TEXT("commits"))
																   ->GetArrayField(TEXT("items"));
	
		TArray<FSpeckleCommit> ArrayOfCommits;
		ArrayOfCommits.Reserve(CommitsArrJSON.Num());
	
		for (const TSharedPtr<FJsonValue> v : CommitsArrJSON)
		{
			FSpeckleCommit Commit = FSpeckleCommit(v);
			ArrayOfCommits.Add( Commit );
		}

		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully. Fetched %d items"), *RequestLogName, ArrayOfCommits.Num());
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(ArrayOfCommits), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};
	
	
	const FHttpRequestRef Request = CreateGraphQLRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	SendGraphQLRequest(Request, RequestLogName, OnError);
}


void FClientAPI::FetchGlobals(const FString& ServerUrl, const FString& AuthToken,
	const FString& StreamId, const FString& ReferencedObjectId,
    const FFetchGlobalsDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	const FString RequestLogName = FString(__FUNCTION__);
	
	FString PostPayload = "{\"query\": \"query{stream (id:\\\"" + StreamId +
							 "\\\"){id name description updatedAt createdAt role isPublic object(id:\\\"" +
								ReferencedObjectId +
							    	"\\\"){id data}}}\"}";
	
	auto OnError = [=](const FString& Message) mutable 
	{
		ensureAlwaysMsgf(OnErrorAction.ExecuteIfBound(Message), TEXT("%s: Unhandled error - %s"), *RequestLogName , *Message);
	};
	
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(CheckRequestFailed(bWasSuccessful, Response, RequestLogName, OnError)) return;
		
		TSharedPtr<FJsonObject> Obj;
		if(!GetResponseAsJSON(Response, RequestLogName, Obj, OnError)) return;
		
		TSharedPtr<FJsonObject> GlobalsJSONObject = Obj->GetObjectField(TEXT("data"))
															->GetObjectField(TEXT("stream"))
																->GetObjectField(TEXT("object"))
																	->GetObjectField(TEXT("data"));


		
		
		FSpeckleGlobals Globals = FSpeckleGlobals(GlobalsJSONObject);
		
		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully"), *RequestLogName);
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(Globals), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};
	
	
	const FHttpRequestRef Request = CreateGraphQLRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	SendGraphQLRequest(Request, RequestLogName, OnError);
}

void FClientAPI::FetchUserData(const FString& ServerUrl, const FString& AuthToken,
    const FFetchUserDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	const FString RequestLogName = FString(__FUNCTION__);
	
	const FString PostPayload =
		"{\"query\": \"query{user{id name email company role suuid bio profiles avatar}}\"}"; 
	
	
	auto OnError = [=](const FString& Message) mutable 
	{
		UE_LOG(LogSpeckle, Warning, TEXT("%s: failed - %s"), *RequestLogName , *Message);
		ensureAlwaysMsgf(OnErrorAction.ExecuteIfBound(Message), TEXT("%s: Unhandled error - %s"), *RequestLogName , *Message);
	};
	
	auto ResponseHandler = [=](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) mutable 
	{
		if(CheckRequestFailed(bWasSuccessful, Response, RequestLogName, OnError)) return;
		
		TSharedPtr<FJsonObject> Obj;
		if(!GetResponseAsJSON(Response, RequestLogName, Obj, OnError)) return;
		
		TSharedPtr<FJsonObject> UserJSONObject = Obj->GetObjectField(TEXT("data"))
													    ->GetObjectField(TEXT("user"));
		
		FSpeckleUser MyUserData = FSpeckleUser( UserJSONObject );
		
		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully"), *RequestLogName);
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(MyUserData), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};

	const FHttpRequestRef Request = CreateGraphQLRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	SendGraphQLRequest(Request, RequestLogName, OnError);
}



bool FClientAPI::GetResponseAsJSON(const FHttpResponsePtr Response, const FString& RequestLogName, TSharedPtr<FJsonObject>& OutObject, const TFunctionRef<void(const FString& Message)> OnErrorAction)
{

	const FString JsonResponse = Response->GetContentAsString();

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonResponse);
	
	if(!FJsonSerializer::Deserialize(Reader, OutObject))
	{
		const FString Message = FString::Printf(
			TEXT("Recieved a response from \"%s\" for \"%s\" request, but the response failed to deserialize: %s"),
			*Response->GetURL(), *RequestLogName, *JsonResponse);
		OnErrorAction(Message);
		return false;
	}


	FString Error;
	if(CheckForOperationErrors(OutObject, Error))
	{
		OnErrorAction(Error);
		return false;
	}
	
	return true;
}

FHttpRequestRef FClientAPI::CreateGraphQLRequest(const FString& ServerUrl, const FString AuthToken, const FString& PostPayload, const FString& Encoding)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(ServerUrl + "/graphql");
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", Encoding);
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);
	Request->SetContentAsString(PostPayload);

	return Request;
}

bool FClientAPI::SendGraphQLRequest(const FHttpRequestRef Request, const FString& RequestName,
 const TFunctionRef<void(const FString& Message)> OnErrorAction)
{
	const bool RequestSent = Request->ProcessRequest();

	if(!RequestSent)
	{
		const FString Message = FString::Printf(TEXT("Request \"%s\" at \"%s\" failed: \nHTTP request failed to start"),
					*RequestName, *Request->GetURL());
		
		OnErrorAction(Message);
	}
	else
	{
		UE_LOG(LogSpeckle, Log, TEXT("POST Request %s to %s was sent, awaiting response"), *RequestName, *Request->GetURL() );
		FAnalytics::TrackEvent("unknown", Request->GetURL(), "NodeRun", TMap<FString, FString> { {"name", RequestName }});
	}
	
	return RequestSent;
}


bool FClientAPI::CheckRequestFailed(bool bWasSuccessful, FHttpResponsePtr Response,  const FString& RequestLogName, const TFunctionRef<void(const FString& Message)> OnErrorAction)
{
	//Check the request was sent
	if(!bWasSuccessful)
	{
		FString Message = FString::Printf(TEXT("Request \"%s\" to \"%s\" was unsuccessful: %s"),
												*RequestLogName, *Response->GetURL(), *Response->GetContentAsString());
		OnErrorAction(Message);
		return true;
	}

	//Check Response code
	const int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		FString Message = FString::Printf(TEXT("Request \"%s\" to \"%s\" failed with HTTP response %d"),
												*RequestLogName, *Response->GetURL(), ResponseCode);
		OnErrorAction(Message);
		return true;
	}
		
	UE_LOG(LogSpeckle, Log, TEXT("Operation %s recieved a response from %s"), *RequestLogName, *Response->GetURL());
	
	return false;
}

bool FClientAPI::CheckForOperationErrors(const TSharedPtr<FJsonObject> GraphQLResponse, FString& OutErrorMessage)
{
	check(GraphQLResponse != nullptr);
	
	bool WasError = false;
	const TArray<TSharedPtr<FJsonValue>>* Errors;
	if(GraphQLResponse->TryGetArrayField(TEXT("Errors"), Errors))
	{
		for(const TSharedPtr<FJsonValue>& e : *Errors)
		{
			FString Message;
			const TSharedPtr<FJsonObject>* ErrorObject;
			bool HadMessage = e->TryGetObject(ErrorObject)
				&& (*ErrorObject)->TryGetStringField("message", Message);
			if(!HadMessage)
			{
				Message = "An operation error occured but had no message!\n";
				UE_LOG(LogSpeckle, Warning, TEXT("%s"), *Message);
			}
			
			OutErrorMessage.Append(Message + "\n");
			WasError = true;
		}
	}
	return WasError;
}

