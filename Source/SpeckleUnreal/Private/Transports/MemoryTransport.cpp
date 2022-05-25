// Fill out your copyright notice in the Description page of Project Settings.


#include "Transports/MemoryTransport.h"

bool UMemoryTransport::HasObject(const FString& ObjectId) const
{
	return SpeckleObjects.Contains(ObjectId);
}

TSharedPtr<FJsonObject> UMemoryTransport::GetSpeckleObject(const FString& ObjectId) const
{
	return SpeckleObjects.FindRef(ObjectId);
}

void UMemoryTransport::SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject)
{
	SpeckleObjects.Add(ObjectId, SerializedObject);
}
