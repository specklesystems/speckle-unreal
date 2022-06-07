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
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ID;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Message;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString BranchName;
	

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString AuthorName;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString AuthorId;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString AuthorAvatar;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString CreatedAt;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString SourceApplication;
	
    UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ReferenceObjectID;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString TotalChildrenCount;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FString> Parents;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
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
		ensureAlways(obj->TryGetStringField("id", ID));
		obj->TryGetStringField("message", Message);
		obj->TryGetStringField("branchName", BranchName);
		obj->TryGetStringField("authorName", AuthorName);
		obj->TryGetStringField("authorId", AuthorId);
		obj->TryGetStringField("authorAvatar", AuthorAvatar);
		obj->TryGetStringField("createdAt", CreatedAt);
		obj->TryGetStringField("sourceApplication", SourceApplication);
		obj->TryGetStringField("totalChildrenCount", TotalChildrenCount);
		ensureAlways(obj->TryGetStringField("referencedObject", ReferenceObjectID));
		obj->TryGetStringArrayField("parents", Parents);
		obj->TryGetStringField("commentCount", CommentCount); //TODO is this a prop?

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