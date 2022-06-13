// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "Interfaces/IHttpRequest.h"

DECLARE_DELEGATE_OneParam(FErrorDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FAPIResponceDelegate, const FString&);

/**
* C++ wrapper of Speckle's GraphQL API
* Encapsulates sending HTTP requests with GraphQL Queries for specific resources
*
* Used to send GraphQL requests for commits, branches, streams, user info, collaborators etc.
*
* See `API/Operations` for usage examples
*/
class FClientAPI
{

 public:
	
	/**
	 * Creates and sends a graphql query to the specified url.
	 * Will check HTTP response for errors,
	 * and unpack the response "data" object.
	 * @param ServerUrl URL of the Speckle Server endpoint
	 * @param AuthToken 
	 * @param ResponsePropertyName Property name of the requested object. This will be the first name in the query (e.g. "stream", "streams", "user", etc)
	 * @param PostPayload The POST payload containing the GraphQL request
	 * @param RequestLogName Friendly name for this request (for error handling and logging)
	 * @param OnCompleteAction Callback invoked on successful completion of the request, with the request object (as a JSON string)
	 * @param OnErrorAction Callback invoked on any fatal error with, the error message.
	 */
	static void MakeGraphQLRequest(const FString& ServerUrl, const FString& AuthToken, const FString& ResponsePropertyName,
	                               const FString& PostPayload, const FString& RequestLogName,
	                               const FAPIResponceDelegate OnCompleteAction, const FErrorDelegate OnErrorAction);

	

//protected:
	
	/// Helper function for creating post requests with a GraphQL query
	static FHttpRequestRef CreatePostRequest(const FString& ServerUrl, FString AuthToken, const FString& PostPayload,
	                                         const FString& Encoding = TEXT("gzip"));

 	static bool GetResponseAsJSON(const FHttpResponsePtr Response, const FString& RequestLogName, TSharedPtr<FJsonObject>& OutObject, const TFunctionRef<void(const FString& Message)> OnErrorAction);

	static bool CheckForOperationErrors(const TSharedPtr<FJsonObject> GraphQLResponse, FString& OutErrorMessage);
 	static bool CheckRequestFailed(bool bWasSuccessful, FHttpResponsePtr Response,  const FString& RequestName, const TFunctionRef<void(const FString& Message)> OnErrorAction);
 	
 };