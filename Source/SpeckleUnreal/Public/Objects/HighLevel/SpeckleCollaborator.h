#pragma once

#include "CoreMinimal.h"
#include "LogSpeckle.h"
#include "SpeckleCollaborator.generated.h"

/*
* Struct that holds all the properties required
* from a Collaborator in a Stream
* received from GraphQL.#pragma once#include "CoreMninimal.h"#include "FSpeckleBranch.generategd.h"
*/
USTRUCT(BlueprintType)
struct FSpeckleCollaborator
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

	UPROPERTY(BlueprintReadWrite)
	FString Email;

	FSpeckleCollaborator(){};

	FSpeckleCollaborator(const TSharedPtr<FJsonValue> CollaboratorsAsJSONValue)
	{
		UE_LOG(LogSpeckle,Log,TEXT(" - - - - - - Collaborators Final - -- - - - - ")  );
		
		Id = CollaboratorsAsJSONValue->AsObject()->GetStringField("id");
		Name = CollaboratorsAsJSONValue->AsObject()->GetStringField("name");
		Company = CollaboratorsAsJSONValue->AsObject()->GetStringField("company");
		Role = CollaboratorsAsJSONValue->AsObject()->GetStringField("role");
		Avatar= CollaboratorsAsJSONValue->AsObject()->GetStringField("avatar");
		Email = Name + FString(TEXT("@")) + Company + FString(TEXT(".com"));   // CollaboratorsAsJSONValue->AsObject()->GetStringField("email");
	}
	
	FSpeckleCollaborator(const FString& Id, const FString& Name, const FString& Company, const FString& Role, const FString& Avatar)
		: Id(Id),
		Name(Name),
		Company(Company),
		Role(Role),
		Avatar(Avatar)
	{
	}
};