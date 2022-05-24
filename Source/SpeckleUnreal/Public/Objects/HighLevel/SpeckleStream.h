#pragma once

#include "CoreMinimal.h"
#include "LogSpeckle.h"
#include "SpeckleCollaborator.h"

#include "SpeckleStream.generated.h"

/*
* Class that holds all the properties required
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

	// UPROPERTY(BlueprintReadWrite)
	// bool IsStreamPublic;

	UPROPERTY(BlueprintReadWrite)
	FString Role;

	UPROPERTY(BlueprintReadWrite)
	FString	CreatedAt;

	UPROPERTY(BlueprintReadWrite)
	FString UpdatedAt;

	UPROPERTY(BlueprintReadWrite)
	TArray<FSpeckleCollaborator> Collaborators;


	void DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj) const
	{
		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
		UE_LOG(LogTemp, Log, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
	}
	
	FSpeckleStream(const TSharedPtr<FJsonValue> StreamAsJSONValue)
	{
		TSharedPtr<FJsonObject> Obj = StreamAsJSONValue->AsObject();
		DisplayAsString("Collaborators insider --->", Obj);
		
		ID = Obj->GetStringField("id");
		Name = Obj->GetStringField("name");
		Description = Obj->GetStringField("description");
		UpdatedAt = Obj->GetStringField("updatedAt");
		CreatedAt = Obj->GetStringField("createdAt");
		Role = Obj->GetStringField("role");

		TArray<TSharedPtr<FJsonValue>> CollaboratorsArrJSONValues = Obj->GetArrayField("collaborators");

		for (TSharedPtr<FJsonValue> collaboratorJSONValue : CollaboratorsArrJSONValues)
		{
			FSpeckleCollaborator Collaborator = FSpeckleCollaborator( collaboratorJSONValue );
			Collaborators.Add( Collaborator );
		}

		
		//IsStreamPublic = StreamAsJSONValue->AsObject()->GetBoolField("isPublic");
	
		//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::White, RoleUser);
	}

	FSpeckleStream(){};
	//
	// FSpeckleStream(const FString& ID, const FString& Name, const FString& Description):
	// 		ID(ID), Name(Name), Description(Description){};
	//
	// FSpeckleStream( const FString& ID,
	// 				const FString& Name,
	// 				const FString& Description,
	// 				const bool& IsPublic,
	// 				const FString& Role,
	// 				const FString& CreatedAt,
	// 				const FString& UpdatedAt):
	// 		ID(ID),
	// 		Name(Name),
	// 		Description(Description),
	// 		IsPublic(IsPublic),
	// 		Role(Role),
	// 		CreatedAt(CreatedAt),
	// 		UpdatedAt(UpdatedAt){}
};