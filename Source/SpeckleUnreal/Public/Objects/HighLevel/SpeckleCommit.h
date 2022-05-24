#pragma once

#include "CoreMinimal.h"
#include "SpeckleCommit.generated.h"

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
	FString ID;
	
    UPROPERTY(BlueprintReadWrite)
	FString ReferenceObjectID;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FString BranchName;

	UPROPERTY(BlueprintReadWrite)
	FString SourceApplication;

	UPROPERTY(BlueprintReadWrite)
	FString TotalChildrenCount;

	UPROPERTY(BlueprintReadWrite)
	FString Parents;

	UPROPERTY(BlueprintReadWrite)
	FString AuthorId;

	UPROPERTY(BlueprintReadWrite)
	FString AuthorName;
	
	UPROPERTY(BlueprintReadWrite)
	FString AuthorAvatar;

	UPROPERTY(BlueprintReadWrite)
	FString CreatedAt;

	UPROPERTY(BlueprintReadWrite)
	FString CommentCount;
	
	FSpeckleCommit(){};

	FSpeckleCommit(const TSharedPtr<FJsonValue> CommitAsJSONValue)
	{
		UE_LOG(LogSpeckle, Log, TEXT("-----------> Speckle Commit Assignment 1"));
		
		ID = CommitAsJSONValue->AsObject()->GetStringField("id");

		UE_LOG(LogSpeckle, Log, TEXT("-----------> Speckle Commit Assignment 2"));
		
		ReferenceObjectID = CommitAsJSONValue->AsObject()->GetStringField("referencedObject");
		Message = CommitAsJSONValue->AsObject()->GetStringField("message");
		SourceApplication = CommitAsJSONValue->AsObject()->GetStringField("sourceApplication");
		TotalChildrenCount= CommitAsJSONValue->AsObject()->GetStringField("totalChildrenCount");
		BranchName = CommitAsJSONValue->AsObject()->GetStringField("branchName");
		Parents = CommitAsJSONValue->AsObject()->GetStringField("parents");

		UE_LOG(LogSpeckle, Log, TEXT("-----------> Speckle Commit Assignment 5"));
		
		AuthorId = CommitAsJSONValue->AsObject()->GetStringField("authorId");
		AuthorName = CommitAsJSONValue->AsObject()->GetStringField("authorName");
		//AuthorAvatar = CommitAsJSONValue->AsObject()->GetStringField("authorAvatar");
		CreatedAt = CommitAsJSONValue->AsObject()->GetStringField("createdAt");
		CommentCount = CommitAsJSONValue->AsObject()->GetStringField("commentCount");

		UE_LOG(LogSpeckle, Log, TEXT("-----------> Speckle Commit Assignment 10"));
	}
	

	// FSpeckleCommit(const FString& ReferenceObjectID, const FString& Author, const FString& Message, const FString& BranchName)
	// : ReferenceObjectID(ReferenceObjectID),
	// Author(Author),
	// Message(Message),
	// BranchName(BranchName)
	// {
	// }

	// FSpeckleCommit(const FString& ReferenceObjectID, const FString& AuthorName, const FString& Message, const FString& BranchName,
	// 	const FString& Id, const FString& SourceApplication, const FString& TotalChildrenCount, const FString& Parents,
	// 	const FString& AuthorId, const FString& AuthorAvatar, const FString& CreatedAt)
 //        : ReferenceObjectID(ReferenceObjectID),
 //          Author(Author),
 //          Message(Message),
	// 	  BranchName(BranchName),
	// 	  ID(Id),
	// 	  SourceApplication(SourceApplication),
	// 	  TotalChildrenCount(TotalChildrenCount),
	// 	  Parents(Parents),
	// 	  AuthorId(AuthorId),
	// 	  AuthorAvatar(AuthorAvatar),
	// 	  CreatedAt(CreatedAt)
	// {
	// }

	//Operators overloading
	
	// FORCEINLINE bool operator==(const FSpeckleCommit &Other) const
	// {
	// 	return ReferenceObjectID == Other.ReferenceObjectID;
	// }
};