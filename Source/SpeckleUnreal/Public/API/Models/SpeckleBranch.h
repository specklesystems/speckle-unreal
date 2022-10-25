
#pragma once

#include "CoreMinimal.h"
#include "SpeckleCommit.h"
#include "SpeckleBranch.generated.h"

/*
* GraphQL model for Branch data
* Properties are only when they explicitly requested (through the GraphQL request)
* see https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
*/
USTRUCT(BlueprintType)
struct FSpeckleBranch
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ID;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Description;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleCommits Commits;
	
};

USTRUCT(BlueprintType)
struct FSpeckleBranches
{
	GENERATED_BODY();
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	int32 TotalCount = 0;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Cursor;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	TArray<FSpeckleBranch> Items;
};
