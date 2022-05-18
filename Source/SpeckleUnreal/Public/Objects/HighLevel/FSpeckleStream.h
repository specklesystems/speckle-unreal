#pragma once

#include "CoreMinimal.h"
#include "FSpeckleStream.generated.h"

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