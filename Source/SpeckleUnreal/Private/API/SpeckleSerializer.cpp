#include "API/SpeckleSerializer.h"

#include "Objects/Base.h"
#include "LogSpeckle.h"
#include "Conversion/ConversionUtils.h"
#include "Objects/ObjectModelRegistry.h"
#include "Transports/Transport.h"


UBase* USpeckleSerializer::DeserializeBase(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(Obj == nullptr) return nullptr;

	{ // Handle Detached Objects
		TSharedPtr<FJsonObject> DetachedObject;
		if(UConversionUtils::ResolveReference(Obj, ReadTransport, DetachedObject))
		{
			return DeserializeBase(DetachedObject, ReadTransport);
		}
	}
	
	FString SpeckleType;	
	if (!Obj->TryGetStringField("speckle_type", SpeckleType)) return nullptr;
	FString ObjectId = "";	
	Obj->TryGetStringField("id", ObjectId);
		
	TSubclassOf<UBase> BaseType = UObjectModelRegistry::FindClosestType(SpeckleType);
	
	if(BaseType == nullptr)
	{
		UE_LOG(LogSpeckle, Verbose, TEXT("Skipping deserialization of %s %s: Unrecognised SpeckleType"), *SpeckleType, *ObjectId );
		BaseType = UBase::StaticClass();
	}
	
	UBase* Base =  NewObject<UBase>(GetTransientPackage(), BaseType);
	if(Base->Parse(Obj, ReadTransport)) return Base;

	//TODO maybe we try the next closest type here, rather than failing after the first try.
	UE_LOG(LogSpeckle, Verbose, TEXT("Skipping deserialization of %s %s: Object could not be deserialised to closest type %s"), *SpeckleType, *ObjectId, *BaseType->GetName());
	return nullptr;
}

UBase* USpeckleSerializer::DeserializeBaseById(const FString& ObjectId,
	const TScriptInterface<ITransport> ReadTransport)
{
	auto Obj = ReadTransport->GetSpeckleObject(ObjectId);
	return DeserializeBase(Obj, ReadTransport);
}
