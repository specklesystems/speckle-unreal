#pragma once

#include "CoreMinimal.h"
#include "FSpeckleActivity.generated.h"

/*
* Struct that holds all the properties required
* for the User Activities
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleActivity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString ActionType;

	UPROPERTY(BlueprintReadWrite)
	FString Time;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FString StreamId;

	UPROPERTY(BlueprintReadWrite)
	FString ResourceType;

	UPROPERTY(BlueprintReadWrite)
	FString ResourceId;

	UPROPERTY(BlueprintReadWrite)
	FString Info;

	FSpeckleActivity(){};

	FSpeckleActivity(const FString& ActionType, const FString& Time, const FString& Message,
		const FString& StreamId, const FString& ResourceType, const FString& ResourceId,
		const FString& Info):
		ActionType(ActionType), Time(Time), Message(Message), StreamId(StreamId),
		ResourceType(ResourceType), ResourceId(ResourceId), Info(Info){}
};