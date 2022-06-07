#pragma once

#include "CoreMinimal.h"
#include "SpeckleCollaborator.h"
#include "SpeckleBranch.generated.h"


/*
* Struct that holds all the properties required
* from a speckle Branch
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleBranch
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ID;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Author;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Commits;

	void DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj) const
	{
		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
		UE_LOG(LogTemp, Log, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
	}

	FSpeckleBranch(const TSharedPtr<FJsonObject> Obj)
	{
		ensureAlways(Obj->TryGetStringField("id", ID));
		ensureAlways(Obj->TryGetStringField("name", Name));
		Obj->TryGetStringField("description", Description);
	}
	
	FSpeckleBranch(){};
	//
	// FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description):
	// 	ID(ID), Name(Name), Description(Description){}
	//
	// FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description, const FString& Author, const FString& Commits):
	// 	ID(ID), Name(Name), Description(Description), Author(Author), Commits(Commits){}
};