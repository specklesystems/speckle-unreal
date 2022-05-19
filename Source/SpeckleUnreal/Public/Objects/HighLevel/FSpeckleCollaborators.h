#pragma once

#include "CoreMinimal.h"
#include "FSpeckleCollaborators.generated.h"

/*
* Struct that holds all the properties required
* from a Collaborator in a Stream
* received from GraphQL.#pragma once#include "CoreMninimal.h"#include "FSpeckleBranch.generategd.h"
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