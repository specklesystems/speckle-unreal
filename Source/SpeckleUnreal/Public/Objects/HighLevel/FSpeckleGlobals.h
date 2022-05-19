#pragma once

#include "CoreMinimal.h"
#include "FSpeckleGlobals.generated.h"


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