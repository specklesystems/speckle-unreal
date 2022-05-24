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



	void DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj) const
	{
		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
		UE_LOG(LogTemp, Log, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
	}
	
	FSpeckleCommit(){};

	FSpeckleCommit(const TSharedPtr<FJsonValue> CommitAsJSONValue)
	{
		TSharedPtr<FJsonObject> obj = CommitAsJSONValue->AsObject();
		ID = obj->GetStringField("id");
		ReferenceObjectID = obj->GetStringField("referencedObject");
		Message = obj->GetStringField("message");
		SourceApplication = obj->GetStringField("sourceApplication");
		TotalChildrenCount= obj->GetStringField("totalChildrenCount");
		BranchName = obj->GetStringField("branchName");
		Parents = obj->GetStringField("parents");
		AuthorId = obj->GetStringField("authorId");
		AuthorName = obj->GetStringField("authorName");
		//AuthorAvatar = CommitAsJSONValue->AsObject()->GetStringField("authorAvatar");
		CreatedAt = obj->GetStringField("createdAt");
		CommentCount = obj->GetStringField("commentCount");

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