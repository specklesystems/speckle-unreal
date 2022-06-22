// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "SpeckleStreamAPIOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAPIResponseHandler, FString, JsonResponse, FString, ErrorMessage);

/**
 * Allows sending GraphQL queries to the Speckle GraphQL API
 * 
 */
UCLASS()
class SPECKLEUNREAL_API USpeckleStreamAPIOperation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
    UPROPERTY(BlueprintAssignable)
    FAPIResponseHandler OnReceiveSuccessfully;

	UPROPERTY(BlueprintAssignable)
    FAPIResponseHandler OnError;

	/**
	 * Sends an GraphQL queries to a speckle server's "/graphql" endpoint
	 * E.g. for fetching stream/branch/commit/user info.
	 *
	 * @param	ServerUrl				The URL of the Speckle server
	 * @param	AuthToken				The auth token of the user (optional for unprivilaged requests)
	 * @param	GraphQlQuery			The GraphQl query (starting with `query{ }`)
	 * @param	ResponsePropertyName	The name of the root property being requested.  (e.g. for a "query{user{...}" query, then the property name is  "user")
	 * @param	RequestLogName			A friendly name for this request (logging and analytics) 
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category = "Speckle|Operations")
	static USpeckleStreamAPIOperation* SpeckleStreamAPIOperation(
		const FString& ServerUrl,
		const FString& AuthToken,
		const FString& GraphQlQuery,
		const FString& ResponsePropertyName,
		const FString& RequestLogName);
	
	virtual void Activate() override;
	
protected:
	void Request();
	
	FString ServerUrl;
	FString AuthToken;
	FString Query;
	FString RequestLogName;
	FString ResponsePropertyName;
	
	void HandleReceive(const FString& ResponseJson);
	
	void HandleError(const FString& Message);

};
