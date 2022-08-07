
#pragma once

#include "CoreMinimal.h"
#include "SpeckleCollaborator.generated.h"

/*
* GraphQL model for Collaborator data
* Properties are only when they explicitly requested (through the GraphQL request)
* see https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
*/
USTRUCT(BlueprintType)
struct FSpeckleCollaborator
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Id;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Role;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Avatar;
	
};