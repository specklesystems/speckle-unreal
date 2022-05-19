// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "FSpeckleUser.generated.h"

/*
* Struct that holds all the properties required
* for a User
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleUser
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Company;

	UPROPERTY(BlueprintReadWrite)
	FString Role;

	UPROPERTY(BlueprintReadWrite)
	FString Id;

	UPROPERTY(BlueprintReadWrite)
	FString Suuid;
	
	UPROPERTY(BlueprintReadWrite)
	FString Email;
	
	UPROPERTY(BlueprintReadWrite)
	FString Bio;
	
	UPROPERTY(BlueprintReadWrite)
	FString Avatar;

	UPROPERTY(BlueprintReadWrite)
	FString Profiles;

	UPROPERTY(BlueprintReadWrite)
	FString ActivityCollection;

	UPROPERTY(BlueprintReadWrite)
	FString TimelineCollection;


	FSpeckleUser(){};
	
	FSpeckleUser(const FString& Name, const FString& Company, const FString& Role,
				 const FString& Avatar, const FString& Profiles, const FString& ActivityCollection,
				 const FString& TimelineCollection):
		Name(Name), Company(Company), Role(Role), Avatar(Avatar), Profiles(Profiles),
		ActivityCollection(ActivityCollection), TimelineCollection(TimelineCollection){}
};