#include "API/SpeckleSerializer.h"

#include "Objects/Base.h"
#include "LogSpeckle.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Objects/ObjectModelRegistry.h"
#include "Templates/SubclassOf.h"
#include "Transports/Transport.h"
#include "UObject/Package.h"


// Create the Deserialization Base
TArray<FSpeckleStream> USpeckleSerializer::DeserializeListOfStreams(const TSharedPtr<FJsonObject> Obj,
										            const TScriptInterface<ITransport> ReadTransport)
{

//	TSharedPtr<FJsonObject> a = ReadTransport->GetSpeckleObject(Obj);

	//---------------------------------
	
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReadTransport->ResponseListOfStreamsSerialized);


	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON 2: %s"), *ReadTransport->ResponseListOfStreamsSerialized);

	
	TArray<FSpeckleStream> ArrayOfStreams;

	TSharedPtr<FJsonObject> JsonObject;
	
	ArrayOfStreams.Empty();
	
	// //Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		for(const auto& pair:JsonObject->Values)
		{
			// nested objects
			auto StreamsArr = JsonObject->GetObjectField(TEXT("data"))
																	->GetObjectField(TEXT("user"))
																	   ->GetObjectField(TEXT("streams"))
																			->GetArrayField(TEXT("items"));

			// FString OutputString;
			// TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			// FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
			
			for (auto s : StreamsArr)
			{
				auto ID = s->AsObject()->GetStringField("id");

					
					
				auto Name = s->AsObject()->GetStringField("name");

				UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON 111: %s"), *Name);
					
				auto Description = s->AsObject()->GetStringField("description");
				auto UpdatedAt = s->AsObject()->GetStringField("updatedAt");
				auto CreatedAt = s->AsObject()->GetStringField("createdAt");
				auto RoleUser = s->AsObject()->GetStringField("role");
				auto IsPublic = s->AsObject()->GetBoolField("isPublic");

				//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::White, RoleUser);
				auto Stream = FSpeckleStream(ID, Name, Description, IsPublic, RoleUser, CreatedAt, UpdatedAt);
				
				ArrayOfStreams.Add(Stream);
			}
		}
	}


	return ArrayOfStreams;
	//-----------------------------------------



	
}


// Create the Deserialization Base
UBase* USpeckleSerializer::DeserializeBase(const TSharedPtr<FJsonObject> Obj,
										   const TScriptInterface<ITransport> ReadTransport)
{
	if(Obj == nullptr) return nullptr;

	{ // Handle Detached Objects
		TSharedPtr<FJsonObject> DetachedObject;
		if(USpeckleObjectUtils::ResolveReference(Obj, ReadTransport, DetachedObject))
		{
			return DeserializeBase(DetachedObject, ReadTransport);
		}
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
