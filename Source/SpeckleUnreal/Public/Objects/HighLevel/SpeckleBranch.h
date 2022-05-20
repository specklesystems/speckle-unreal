#pragma once

#include "CoreMinimal.h"
#include "SpeckleBranch.generated.h"


/*
* Struct that holds all the properties required
* from a speckle Branch
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleBranch
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString ID;

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Description;

	UPROPERTY(BlueprintReadWrite)
	FString Author;

	UPROPERTY(BlueprintReadWrite)
	FString Commits;


	FSpeckleBranch(const TSharedPtr<FJsonValue> StreamAsJSONValue)
	{
		ID = StreamAsJSONValue->AsObject()->GetStringField("id");
		Name = StreamAsJSONValue->AsObject()->GetStringField("name");
		Description = StreamAsJSONValue->AsObject()->GetStringField("description");
		//Author = StreamAsJSONValue->AsObject()->GetStringField("author");
		//Commits = StreamAsJSONValue->AsObject()->GetStringField("commits");
	}
	
	FSpeckleBranch(){};
	
	FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description):
		ID(ID), Name(Name), Description(Description){}
	
	FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description, const FString& Author, const FString& Commits):
		ID(ID), Name(Name), Description(Description), Author(Author), Commits(Commits){}
};