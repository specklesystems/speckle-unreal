
#include "API/ClientAPI.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "LogSpeckle.h"
#include "Mixpanel.h"

void FClientAPI::MakeGraphQLRequest(const FString& ServerUrl, const FString& AuthToken,
	const FString& ResponsePropertyName,
	const FString& PostPayload, const FString& RequestLogName,
	const FAPIResponceDelegate OnCompleteAction, const FErrorDelegate OnErrorAction)
{
	
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

		const TSharedPtr<FJsonObject>* DataObjectPtr;
		if(!Obj->TryGetObjectField(TEXT("data"), DataObjectPtr))
		{
			OnError(TEXT("%s responce was invalid, expected to find a \"data\" property in response"));
			return;
		}
		const TSharedPtr<FJsonObject> DataObject = *DataObjectPtr;
		
		const TSharedPtr<FJsonObject>* RequestedJson;
		if(!DataObject->TryGetObjectField(ResponsePropertyName, RequestedJson))
		{
			//Requested property has a null or non-object value
			if(DataObject->HasField(ResponsePropertyName))
			{
				FString Message = FString::Printf(TEXT("%s: Response data contained the requested property \"%s\", but the value was null was or not an object."),
				*RequestLogName, *ResponsePropertyName);
				OnError(Message);
				return;
			}

			//Requested property was missing
			TArray<FString> Options;
			Options.Reserve(DataObject->Values.Num());
			for(const auto& Properties : DataObject->Values)
			{
				Options.Add(FString::Printf(TEXT("\"%s\" "), *Properties.Key));
			}
			
			FString Message = FString::Printf(TEXT("%s: Could not find the requested property name \"%s\" in responce data.\n Got { %s} instead."),
				*RequestLogName, *ResponsePropertyName, *FString::Join(Options, TEXT(", ")));
			OnError(Message);
			return;
		}
		
		FString OutputString;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		ensureAlways(FJsonSerializer::Serialize(RequestedJson->ToSharedRef(), Writer));

		// Commented out, deserialisation done by caller
		//void* DeserializedData;
		//FSpeckleUser DeserializedData = FSpeckleUser( UserJSONObject );
		// if(!FJsonObjectConverter::JsonObjectToUStruct(*RequestedJson, OutStructType::StaticStruct(), &DeserializedData, 0, 0))
		// {
		// 	OnError("Failed to deserialize user object");
		// 	return;
		// }
		
		UE_LOG(LogSpeckle, Log, TEXT("Operation %s completed successfully"), *RequestLogName);
		ensureAlwaysMsgf(OnCompleteAction.ExecuteIfBound(OutputString), TEXT("%s: Complete handler was not bound properly"), *RequestLogName);
	};

	const FHttpRequestRef Request = CreatePostRequest(ServerUrl, AuthToken, PostPayload);
	Request->OnProcessRequestComplete().BindLambda(ResponseHandler);
	
	if(Request->ProcessRequest())
	{
		UE_LOG(LogSpeckle, Log, TEXT("POST Request %s to %s was sent, awaiting response"), *RequestLogName, *Request->GetURL() );
	}
	else
	{
		UE_LOG(LogSpeckle, Warning, TEXT("POST Request %s to %s failed to send"), *RequestLogName, *Request->GetURL() );
	}
	
	FAnalytics::TrackEvent(Request->GetURL(), TEXT("NodeRun"), TMap<FString, FString> { {TEXT("name"), __FUNCTION__}});
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

FHttpRequestRef FClientAPI::CreatePostRequest(const FString& ServerUrl, const FString AuthToken, const FString& PostPayload, const FString& Encoding)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(ServerUrl + TEXT("/graphql"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Accept-Encoding"), Encoding);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	if(!AuthToken.IsEmpty())
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AuthToken));
	Request->SetHeader(TEXT("apollographql-client-name"), TEXT("Unreal Engine"));
	Request->SetHeader(TEXT("apollographql-client-version"), SPECKLE_CONNECTOR_VERSION);
	Request->SetContentAsString(PostPayload);

	return Request;
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
		FString Message = FString::Printf(TEXT("Request \"%s\" to \"%s\" failed with HTTP response %d - %s"),
												*RequestLogName, *Response->GetURL(), ResponseCode, *Response->GetContentAsString());
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
	if(GraphQLResponse->TryGetArrayField(TEXT("errors"), Errors))
	{
		for(const TSharedPtr<FJsonValue>& e : *Errors)
		{
			FString Message;
			const TSharedPtr<FJsonObject>* ErrorObject;
			bool HadMessage = e->TryGetObject(ErrorObject)
				&& (*ErrorObject)->TryGetStringField(TEXT("message"), Message);
			if(!HadMessage)
			{
				Message = TEXT("An operation error occured but had no message!\n");
				UE_LOG(LogSpeckle, Warning, TEXT("%s"), *Message);
			}
			
			OutErrorMessage.Append(Message + "\n");
			WasError = true;
		}
	}
	return WasError;
}

