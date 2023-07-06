
#pragma once

#include "CoreMinimal.h"
#include "SpeckleStream.h"
#include "SpeckleUser.generated.h"

/*
* GraphQL model for User data
* Properties are only when they explicitly requested (through the GraphQL request)
* see https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
*/
USTRUCT(BlueprintType)
struct FSpeckleUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Id;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Email;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Bio;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Company;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Avatar;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	bool Verified;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Role;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleStreams Streams;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FSpeckleStreams FavoriteStreams;
};