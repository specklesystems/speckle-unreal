#pragma once

#include "CoreMinimal.h"
#include "LogSpeckle.h"

#include "SpeckleStream.generated.h"

/*
* Class that holds all the properties required
* from a speckle Branch
* received from GraphQL.
*/
USTRUCT(BlueprintType)
struct FSpeckleStream 
{
	GENERATED_BODY()
	
public:
	
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

	FSpeckleStream(const TSharedPtr<FJsonValue> StreamAsJSONValue)
	{
		ID = StreamAsJSONValue->AsObject()->GetStringField("id");
		Name = StreamAsJSONValue->AsObject()->GetStringField("name");
		Description = StreamAsJSONValue->AsObject()->GetStringField("description");
		UpdatedAt = StreamAsJSONValue->AsObject()->GetStringField("updatedAt");
		CreatedAt = StreamAsJSONValue->AsObject()->GetStringField("createdAt");
		Role = StreamAsJSONValue->AsObject()->GetStringField("role");
		IsPublic = StreamAsJSONValue->AsObject()->GetBoolField("isPublic");
	
		//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::White, RoleUser);
	}

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