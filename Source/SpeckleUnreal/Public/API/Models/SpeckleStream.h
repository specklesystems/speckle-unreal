// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "SpeckleBranch.h"
#include "SpeckleCollaborator.h"

#include "SpeckleStream.generated.h"

/*
* GraphQL model for Stream data
* Properties are only when they explicitly requested (through the GraphQL request)
* see https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
*/
USTRUCT(BlueprintType)
struct FSpeckleStream 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models", DisplayName="Stream Id")
	FString ID;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString Description;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	bool IsPublic;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString Role;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString	CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString UpdatedAt;

	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FString FavoritedDate;
	
	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	TArray<FSpeckleCollaborator> Collaborators;

	// Object properties are only set if explicitly requested by a custom GraphQL request
	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FSpeckleBranches Branches;

	// Object properties are only set if explicitly requested by a custom GraphQL request
	UPROPERTY(BlueprintReadOnly, DisplayName="Branch (Request Only)", Category="Speckle|API Models", AdvancedDisplay)
	FSpeckleBranch Branch;

	// Object properties are only set if explicitly requested by a custom GraphQL request
	UPROPERTY(BlueprintReadOnly, DisplayName="Commit (Request Only)", Category="Speckle|API Models", AdvancedDisplay)
	FSpeckleCommit Commit;

	// Object properties are only set if explicitly requested by a custom GraphQL request
	UPROPERTY(BlueprintReadOnly, Category="Speckle|API Models")
	FSpeckleCommits Commits;
};

USTRUCT(BlueprintType)
struct FSpeckleStreams
{
	GENERATED_BODY();
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	int32 TotalCount;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Cursor;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FSpeckleStream> Items;
};