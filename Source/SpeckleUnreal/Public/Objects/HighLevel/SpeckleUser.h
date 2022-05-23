// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "SpeckleUser.generated.h"

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
	FString Id;
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Email;
	
	UPROPERTY(BlueprintReadWrite)
	FString Company;

	UPROPERTY(BlueprintReadWrite)
	FString Suuid;
	
	UPROPERTY(BlueprintReadWrite)
	FString Role;
	
	UPROPERTY(BlueprintReadWrite)
	FString Bio;
	
	UPROPERTY(BlueprintReadWrite)
	FString Avatar;

	// UPROPERTY(BlueprintReadWrite)
	// FString Profiles;
	//
	// UPROPERTY(BlueprintReadWrite)
	// FString ActivityCollection;
	//
	// UPROPERTY(BlueprintReadWrite)
	// FString TimelineCollection;

	FSpeckleUser(){};

	FSpeckleUser(TSharedPtr<FJsonObject> MyUserDataJSONObject)
	{
		Id      = *MyUserDataJSONObject->GetStringField("id");
		Name    = *MyUserDataJSONObject->GetStringField("name");
		Company = *MyUserDataJSONObject->GetStringField("company");
		Role    = *MyUserDataJSONObject->GetStringField("role");
		Suuid   = *MyUserDataJSONObject->GetStringField("suuid");
		Email   = *MyUserDataJSONObject->GetStringField("email");
		Bio     = *MyUserDataJSONObject->GetStringField("bio");
		Avatar  = *MyUserDataJSONObject->GetStringField("avatar");
	}

	
	//query{user{id,name,email,company,role,suuid,bio,profiles,avatar}}
	
	
	FSpeckleUser(
				 const FString& Id,
		         const FString& Name,
		         const FString& Email,
		         const FString& Company,
		         const FString& Suuid,
		         const FString& Role,
		         const FString& Bio,
				 const FString& Avatar)
				 :
		Id(Id),
		Name(Name),
		Email(Email),
		Company(Company),
		Suuid(Suuid),
		Role(Role),
		Bio(Bio),
		Avatar(Avatar)
	{}
};