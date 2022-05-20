#include "API/SpeckleSerializer.h"

#include "Objects/Base.h"
#include "LogSpeckle.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Objects/ObjectModelRegistry.h"
#include "Objects/HighLevel/SpeckleStream.h"
#include "Templates/SubclassOf.h"
#include "Transports/Transport.h"
#include "UObject/Package.h"


// Create the Deserialization Base
TArray<FSpeckleStream> USpeckleSerializer::DeserializeListOfStreams(const TSharedPtr<FJsonObject> Obj,
										            const TScriptInterface<ITransport> ReadTransport)
{

	TArray<FSpeckleStream> ArrayOfStreams;

	if(Obj == nullptr)
	{
		UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON OBJ is null"));
		return ArrayOfStreams;
	};
	
	ArrayOfStreams.Empty();
			
	// {"data":{"user":{"streams":{"totalCount":23,"items":[{"id":"2171b53ac6","name":"Test Push","description":"Mindesk code","updatedAt":"2022-05-17T09:10:40.841Z","createdAt":"2022-05-17T07:18:29.541Z","isPublic":false,"role":"stream:owner"},{"id":"a18f8c8569","name":"ZH Villa ","description"  ...}]
	TArray<TSharedPtr<FJsonValue>> StreamsArrJSON = Obj->GetObjectField(TEXT("data"))
															   ->GetObjectField(TEXT("user"))
																  ->GetObjectField(TEXT("streams"))
																	   ->GetArrayField(TEXT("items"));
	for (TSharedPtr<FJsonValue> streamAsJSONValue : StreamsArrJSON)
	{
		FSpeckleStream Stream = FSpeckleStream(streamAsJSONValue);
		ArrayOfStreams.Add(Stream);
	}

	return ArrayOfStreams;
}


// Create the Deserialization Base
UBase* USpeckleSerializer::DeserializeBase(const TSharedPtr<FJsonObject> Obj,
										   const TScriptInterface<ITransport> ReadTransport)
{
	if(Obj == nullptr) return nullptr;

	// Handle Detached Objects
	TSharedPtr<FJsonObject> DetachedObject;
	if(USpeckleObjectUtils::ResolveReference(Obj, ReadTransport, DetachedObject))
	{
			return DeserializeBase(DetachedObject, ReadTransport);
	}
		
	FString SpeckleType;	
	if (!Obj->TryGetStringField("speckle_type", SpeckleType)) return nullptr;
	FString ObjectId = "";	
	Obj->TryGetStringField("id", ObjectId);
		
	TSubclassOf<UBase> BaseType;

	FString WorkingType(SpeckleType);
	
	int32 Tries = 1000;
	while(ensure(Tries-- > 0))
	{
		//Try and deserialize
		if(UObjectModelRegistry::TryGetRegisteredType(WorkingType, BaseType))
		{
			UBase* Base = NewObject<UBase>(GetTransientPackage(), BaseType);
			if(Base->Parse(Obj, ReadTransport))
				return Base; 
		}

		//If we couldn't even deserialize this to a Base
		if(WorkingType == "Base" || BaseType == UBase::StaticClass())
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Skipping deserilization of %s id: %s - object could not be deserilaized to Base"), *SpeckleType, *ObjectId );
			return nullptr;
		}

		//Try the next type
		if(!UObjectModelRegistry::ParentType(WorkingType, WorkingType))
		{
			WorkingType = "Base";
			UE_LOG(LogSpeckle, Verbose, TEXT("Unrecognised SpeckleType %s - Object id: %s Will be deserialized as Base"), *SpeckleType, *ObjectId );
		}
	}
	return nullptr;
}

UBase* USpeckleSerializer::DeserializeBaseById(const FString& ObjectId,
                                               const TScriptInterface<ITransport> ReadTransport)
{
	auto Obj = ReadTransport->GetSpeckleObject(ObjectId);
	return DeserializeBase(Obj, ReadTransport);
}
