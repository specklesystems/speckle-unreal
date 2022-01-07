// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SpeckleSerializerComponent.h"


// Sets default values for this component's properties
USpeckleSerializerComponent::USpeckleSerializerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

UBase* USpeckleSerializerComponent::DeserializeBase(const TSharedPtr<FJsonObject> Object, const TMap<FString, TSharedPtr<FJsonObject>>& JsonStream)
{
	if (!Object->HasField("speckle_type"))
		return nullptr;
	if (Object->GetStringField("speckle_type") == "reference" && Object->HasField("referencedId"))
	{
		if (JsonStream.Contains(Object->GetStringField("referencedId")))
			return DeserializeBase(JsonStream[Object->GetStringField("referencedId")], JsonStream);;
		return nullptr;
	}
	if (!Object->HasField("id") || !Object->HasField("speckle_type"))
		return nullptr;

	const FString ObjectId = Object->GetStringField("id");
	const FString SpeckleType = Object->GetStringField("speckle_type");

	//objectType = Base.
	return nullptr;
}


