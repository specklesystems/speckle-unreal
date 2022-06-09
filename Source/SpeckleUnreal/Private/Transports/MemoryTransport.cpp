// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "Transports/MemoryTransport.h"

#include "LogSpeckle.h"

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
	UE_LOG(LogSpeckle, Verbose, TEXT("Added %s to in memory transport, now %d objects total "), *ObjectId, SpeckleObjects.Num());
}
