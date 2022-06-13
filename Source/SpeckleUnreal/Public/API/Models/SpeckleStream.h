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

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ID;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	bool IsPublic;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Role;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString	CreatedAt;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString UpdatedAt;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString FavoritedDate ;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FSpeckleCollaborator> Collaborators;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleBranches Branches;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleBranch Branch;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleCommit Commit;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
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