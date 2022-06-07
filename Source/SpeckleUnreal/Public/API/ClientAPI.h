// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0
#pragma once
#include "Interfaces/IHttpRequest.h"

struct FSpeckleStream;
struct FSpeckleBranch;
struct FSpeckleCommit;
struct FSpeckleGlobals;
struct FSpeckleUser;

DECLARE_DELEGATE_OneParam(FErrorDelegate, const FString&);
DECLARE_DELEGATE_OneParam(FFetchStreamDelegate, const TArray<FSpeckleStream>&);
DECLARE_DELEGATE_OneParam(FFetchBranchDelegate, const TArray<FSpeckleBranch>&);
DECLARE_DELEGATE_OneParam(FFetchCommitDelegate, const TArray<FSpeckleCommit>&);
DECLARE_DELEGATE_OneParam(FFetchGlobalsDelegate, const FSpeckleGlobals&);
DECLARE_DELEGATE_OneParam(FFetchUserDelegate, const FSpeckleUser&);

 /**
  * C++ wrapper for GraphQL requests
  * See Operations to use with blueprint
  */
 class FClientAPI
 {

 	
 public:
 	
 	
	
 	static void StreamsGet(   const FString& ServerUrl,
 								const FString& AuthToken,
 								const int32 Limit,
								const FFetchStreamDelegate OnCompleteAction,
								const FErrorDelegate OnErrorAction);

 	static void StreamGetBranches(  const FString& ServerUrl,
 		                        const FString& AuthToken,
								const FString& StreamId,
								const int32 Limit,
								const FFetchBranchDelegate OnCompleteAction,
								const FErrorDelegate OnErrorAction);

 	static void StreamGetCommits(   const FString& ServerUrl,
 	                            const FString& AuthToken,
 	                            const FString& StreamId,
 		                        const FString& BranchName,
 		                        const int32 Limit,
								const FFetchCommitDelegate OnCompleteAction,
								const FErrorDelegate OnErrorAction);

 	static void FetchGlobals(	const FString& ServerUrl,
								const FString& AuthToken,
								const FString& StreamId,
								const FString& ReferencedObjectId,
								const FFetchGlobalsDelegate OnCompleteAction,
								const FErrorDelegate OnErrorAction);

 	static void FetchUserData(	const FString& ServerUrl,
								const FString& AuthToken,
								const FFetchUserDelegate OnCompleteAction,
								const FErrorDelegate OnErrorAction);
 	



 	

 	//Utility functions

    static FHttpRequestRef CreateGraphQLRequest(const FString& ServerUrl, FString AuthToken, const FString& PostPayload,
                                                const FString& Encoding = TEXT("gzip"));
 	
 	static bool SendGraphQLRequest(const FHttpRequestRef Request, const FString& RequestName, const TFunctionRef<void(const FString& Message)> OnErrorAction);

 	static bool GetResponseAsJSON(const FHttpResponsePtr Response, const FString& RequestLogName, TSharedPtr<FJsonObject>& OutObject, const TFunctionRef<void(const FString& Message)> OnErrorAction);
 	
 	static bool CheckForOperationErrors(const TSharedPtr<FJsonObject> GraphQLResponse, FString& OutErrorMessage);
 	static bool CheckRequestFailed(bool bWasSuccessful, FHttpResponsePtr Response,  const FString& RequestName, const TFunctionRef<void(const FString& Message)> OnErrorAction);
 	
 };