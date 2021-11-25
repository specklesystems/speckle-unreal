// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MetaInformation.generated.h"

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

/*
* Struct that holds all the properties required
* for the User Activities
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleActivity
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString ActionType;

	UPROPERTY(BlueprintReadWrite)
	FString Time;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FString StreamId;

	UPROPERTY(BlueprintReadWrite)
	FString ResourceType;

	UPROPERTY(BlueprintReadWrite)
	FString ResourceId;

	UPROPERTY(BlueprintReadWrite)
	FString Info;

	FSpeckleActivity(){};

	FSpeckleActivity(const FString& ActionType, const FString& Time, const FString& Message,
		const FString& StreamId, const FString& ResourceType, const FString& ResourceId,
		const FString& Info):
		ActionType(ActionType), Time(Time), Message(Message), StreamId(StreamId),
	    ResourceType(ResourceType), ResourceId(ResourceId), Info(Info){}
};

/*
* Struct that holds all the properties required
* from a speckle Branch
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleStream
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString ID;

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Description;

	UPROPERTY(BlueprintReadWrite)
	bool IsPublic;

	UPROPERTY(BlueprintReadWrite)
	FString Role;

	UPROPERTY(BlueprintReadWrite)
	FString	CreatedAt;

	UPROPERTY(BlueprintReadWrite)
	FString UpdatedAt;
	
	FSpeckleStream(){};

	FSpeckleStream(const FString& ID, const FString& Name, const FString& Description):
			ID(ID), Name(Name), Description(Description){};
	
	FSpeckleStream( const FString& ID,
					const FString& Name,
					const FString& Description,
		            const bool& IsPublic,
		            const FString& Role,
		            const FString& CreatedAt,
					const FString& UpdatedAt):
			ID(ID),
			Name(Name),
			Description(Description),
			IsPublic(IsPublic),
			Role(Role),
			CreatedAt(CreatedAt),
			UpdatedAt(UpdatedAt){}
};

/*
* Struct that holds all the properties required
* from a Collaborator in a Stream
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleCollaborators
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Id;
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	FString Company;

	UPROPERTY(BlueprintReadWrite)
	FString Role;

	UPROPERTY(BlueprintReadWrite)
	FString Avatar;

	FSpeckleCollaborators(){};

	FSpeckleCollaborators(const FString& Id, const FString& Name, const FString& Company, const FString& Role, const FString& Avatar)
		: Id(Id),
		Name(Name),
		Company(Company),
		Role(Role),
		Avatar(Avatar)
	{
	}
};

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

	FSpeckleBranch(){};
	
	FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description):
		ID(ID), Name(Name), Description(Description){}
	
	FSpeckleBranch(const FString& ID, const FString& Name, const FString& Description, const FString& Author, const FString& Commits):
		ID(ID), Name(Name), Description(Description), Author(Author), Commits(Commits){}
};

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

/*
* Struct that holds all the properties required
* from a speckle Branch
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleGlobals
{
	GENERATED_USTRUCT_BODY()
	
    UPROPERTY(BlueprintReadWrite)
	FString ID;

	UPROPERTY(BlueprintReadWrite)
	FString Region;

	UPROPERTY(BlueprintReadWrite)
	float Latitude;

	UPROPERTY(BlueprintReadWrite)
	float Longitude;

	UPROPERTY(BlueprintReadWrite)
	float Height;

	FSpeckleGlobals(): Latitude(0), Longitude(0), Height(0)
	{
	};

	FSpeckleGlobals(const FString& ID, const FString& Region, float Latitude, float Longitude)
	: ID(ID),
	Region(Region),
	Latitude(Latitude),
	Longitude(Longitude)
	{
	}


	
	FSpeckleGlobals(const FString& ID, const FString& Region, float Latitude, float Longitude, float Height)
		: ID(ID),
		  Region(Region),
		  Latitude(Latitude),
		  Longitude(Longitude),
		  Height(Height)	
	{
	}
};

UENUM()
enum ESpeckleItemType {Stream, Commit, Branch};
