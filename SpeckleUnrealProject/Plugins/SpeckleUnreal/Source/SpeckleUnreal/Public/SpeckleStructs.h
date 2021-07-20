// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpeckleStructs.generated.h"

/*
* Struct that holds all the properties required
* from a speckle commit
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleCommit
{
	GENERATED_USTRUCT_BODY()
	
    UPROPERTY(BlueprintReadWrite)
	FString ReferenceObjectID;

	UPROPERTY(BlueprintReadWrite)
	FString AuthorName;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FString BranchName;

	FSpeckleCommit()
	{
	}

	FSpeckleCommit(const FString& ReferenceObjectID, const FString& AuthorName, const FString& Message, const FString& BranchName)
        : ReferenceObjectID(ReferenceObjectID),
          AuthorName(AuthorName),
          Message(Message),
		  BranchName(BranchName)
	{
	}

	//Operators overloading
	
	FORCEINLINE bool operator==(const FSpeckleCommit &Other) const
	{
		return ReferenceObjectID == Other.ReferenceObjectID;
	}
};

/*
* Struct that holds all the properties required
* from a speckle commit
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

	FSpeckleBranch()
	{
	}

	FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description):
        ID(ID), Name(Name), Description(Description){}
};

UENUM()
enum ESpeckleItemType {Stream, Commit, Branch};
