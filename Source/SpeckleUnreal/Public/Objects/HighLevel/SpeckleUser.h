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
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Id;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Email;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Bio;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Company;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Avatar;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Suuid;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Role;
	

	FSpeckleUser(){};
	
	FString DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj) const
    {
    		FString OutputString;
    		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    		FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
    		UE_LOG(LogSpeckle, Log, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
    }

	FSpeckleUser(const TSharedPtr<FJsonObject> MyUserDataJSONObject)
	{
		ensureAlways(MyUserDataJSONObject->TryGetStringField("id", Id));
		MyUserDataJSONObject->TryGetStringField("name", Name);
		MyUserDataJSONObject->TryGetStringField("company", Company);
		MyUserDataJSONObject->TryGetStringField("role", Role);
		MyUserDataJSONObject->TryGetStringField("suuid", Suuid); //TODO Is this a prop?
		MyUserDataJSONObject->TryGetStringField("email", Email);
		MyUserDataJSONObject->TryGetStringField("bio", Bio);
		MyUserDataJSONObject->TryGetStringField("avatar", Avatar);

	}

	
	//query{user{id,name,email,company,role,suuid,bio,profiles,avatar}}
	
	
	FSpeckleUser(
				 const FString& Id,
		         const FString& Email,
		         const FString& Bio,
		         const FString& Company,
		         const FString& Name,
				 const FString& Avatar,
		         const FString& Suuid,
		         const FString& Role)
				 :
		Id(Id),
		Email(Email),
		Name(Name),
		Bio(Bio),
		Company(Company),
		Avatar(Avatar),
		Suuid(Suuid),
		Role(Role)
	{}
};