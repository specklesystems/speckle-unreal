#pragma once

#include "CoreMinimal.h"
#include "FSpeckleCommit.generated.h"

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

	UPROPERTY(BlueprintReadWrite)
	FString Id;

	UPROPERTY(BlueprintReadWrite)
	FString SourceApplication;

	UPROPERTY(BlueprintReadWrite)
	FString TotalChildrenCount;

	UPROPERTY(BlueprintReadWrite)
	FString Parents;

	UPROPERTY(BlueprintReadWrite)
	FString AuthorId;

	UPROPERTY(BlueprintReadWrite)
	FString AuthorAvatar;

	UPROPERTY(BlueprintReadWrite)
	FString CreatedAt;
	
	FSpeckleCommit(){};

	FSpeckleCommit(const FString& ReferenceObjectID, const FString& AuthorName, const FString& Message, const FString& BranchName)
	: ReferenceObjectID(ReferenceObjectID),
	AuthorName(AuthorName),
	Message(Message),
	BranchName(BranchName)
	{
	}

	FSpeckleCommit(const FString& ReferenceObjectID, const FString& AuthorName, const FString& Message, const FString& BranchName,
		const FString& Id, const FString& SourceApplication, const FString& TotalChildrenCount, const FString& Parents,
		const FString& AuthorId, const FString& AuthorAvatar, const FString& CreatedAt)
        : ReferenceObjectID(ReferenceObjectID),
          AuthorName(AuthorName),
          Message(Message),
		  BranchName(BranchName),
		  Id(Id),
		  SourceApplication(SourceApplication),
		  TotalChildrenCount(TotalChildrenCount),
		  Parents(Parents),
		  AuthorId(AuthorId),
		  AuthorAvatar(AuthorAvatar),
		  CreatedAt(CreatedAt)
	{
	}

	//Operators overloading
	
	FORCEINLINE bool operator==(const FSpeckleCommit &Other) const
	{
		return ReferenceObjectID == Other.ReferenceObjectID;
	}
};