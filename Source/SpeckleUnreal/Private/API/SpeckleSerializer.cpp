
#include "API/SpeckleSerializer.h"

#include "Objects/Base.h"
#include "LogSpeckle.h"
#include "Objects/DisplayValueElement.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Objects/ObjectModelRegistry.h"
#include "Templates/SubclassOf.h"
#include "Transports/Transport.h"
#include "UObject/Package.h"


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
	
	FString WorkingType{};
	FString Remainder = FString(SpeckleType);

	int32 Tries = 200;
	while(ensure(Tries-- > 0))
	{
		const bool IsLastChance = !UObjectModelRegistry::SplitSpeckleType(Remainder, Remainder, WorkingType);
		
		if(UObjectModelRegistry::TryGetRegisteredType(WorkingType, BaseType))
		{
			UBase* Base = NewObject<UBase>(GetTransientPackage(), BaseType);
			if(Base->Parse(Obj, ReadTransport))
				return Base; 
		}

		if(IsLastChance)
		{
			//Try a fallback displayValue object
			UBase* Base = NewObject<UBase>(GetTransientPackage(), UDisplayValueElement::StaticClass());
			if(Base->Parse(Obj, ReadTransport))
				return Base;

			//If not, try a regular Base
			Base = NewObject<UBase>(GetTransientPackage(), UBase::StaticClass());
			if(Base->Parse(Obj, ReadTransport))
				return Base;

			UE_LOG(LogSpeckle, Warning, TEXT("Skipping deserilization of %s id: %s - object could not be deserilaized to Base"), *SpeckleType, *ObjectId );
			return nullptr;
		}
		
		//If we couldn't even deserialize this to a Base, something is quite wrong...
		if(WorkingType == "Base" || BaseType == UBase::StaticClass())
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Skipping deserilization of %s id: %s - object could not be deserilaized to Base"), *SpeckleType, *ObjectId );
			return nullptr;
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
