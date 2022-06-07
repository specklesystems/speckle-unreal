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
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString ID;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Description;

	// UPROPERTY(BlueprintReadWrite)
	// bool IsStreamPublic;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString Role;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString	CreatedAt;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
	FString UpdatedAt;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|API Models")
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
		//DisplayAsString("Collaborators insider --->", Obj);

		ensureAlways(Obj->TryGetStringField("id", ID));
		ensureAlways(Obj->TryGetStringField("name", Name));
		Obj->TryGetStringField("description", Description);
		Obj->TryGetStringField("updatedAt", UpdatedAt);
		Obj->TryGetStringField("createdAt", CreatedAt);
		Obj->TryGetStringField("role", Role);
		
		const TArray<TSharedPtr<FJsonValue>>* CollaboratorsArrJSONValues;
		Obj->TryGetArrayField("collaborators", CollaboratorsArrJSONValues);

		for (const TSharedPtr<FJsonValue>& c : *CollaboratorsArrJSONValues)
		{
			const TSharedPtr<FJsonObject>* o;
			if(ensure(c->TryGetObject(o)))
			{
				FSpeckleCollaborator Collaborator = FSpeckleCollaborator(*o);
				Collaborators.Add(Collaborator);
			}
		}
		
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