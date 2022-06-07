#pragma once

#include "CoreMinimal.h"
#include "SpeckleCollaborator.generated.h"

/*
* Struct that holds all the properties required
* from a Collaborator in a Stream
* received from GraphQL.
* See https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Api/GraphQL/Models.cs
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
	
	FSpeckleCollaborator() { }

	FSpeckleCollaborator(const TSharedPtr<FJsonObject> JsonObject)
	{
		ensureAlways(JsonObject->TryGetStringField("id", Id));
		ensureAlways(JsonObject->TryGetStringField("name", Name));
		JsonObject->TryGetStringField("role", Role);
		JsonObject->TryGetStringField("avatar", Avatar);
	}
	
	FSpeckleCollaborator(const FString& Id, const FString& Name, const FString& Company, const FString& Role, const FString& Avatar)
		: Id(Id),
		Name(Name),
		Role(Role),
		Avatar(Avatar)
	{
	}
};