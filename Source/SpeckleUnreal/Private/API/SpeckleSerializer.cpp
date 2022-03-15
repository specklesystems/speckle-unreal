#include "API/SpeckleSerializer.h"

#include "Objects/Base.h"
#include "LogSpeckle.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Objects/ObjectModelRegistry.h"
#include "Transports/Transport.h"


UBase* USpeckleSerializer::DeserializeBase(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
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
