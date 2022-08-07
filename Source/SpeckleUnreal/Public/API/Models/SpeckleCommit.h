
#pragma once

#include "CoreMinimal.h"
#include "SpeckleCommit.generated.h"

/*
* GraphQL model for Commit data
* Properties are only when they explicitly requested (through the GraphQL request)
* see https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
*/
USTRUCT(BlueprintType)
struct FSpeckleCommit
{
	GENERATED_BODY();

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
	
    UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models", DisplayName="Referenced Object Id")
	FString ReferencedObject;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString TotalChildrenCount;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FString> Parents;
	
};

USTRUCT(BlueprintType)
struct FSpeckleCommits
{
	GENERATED_BODY();
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	int32 TotalCount;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Cursor;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FSpeckleCommit> Items;
};
