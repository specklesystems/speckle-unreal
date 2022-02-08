#include "SpeckleUnrealActor.h"
#include "SpeckleUnrealManager.h"
#include "LogSpeckle.h"

#include "Objects/RenderMaterial.h"


void ASpeckleUnrealManager::ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject)
{
	{ // Handle Detached Objects
		TSharedPtr<FJsonObject> DetachedObject;
		if(ResolveReference(SpeckleObject, DetachedObject))
		{
			return ImportObjectFromCache(AOwner, DetachedObject);
		}
	}
	
	const UBase* Base = DeserializeBase(SpeckleObject);

	// Convert Base
	AActor* Native = Converter->ConvertToNative(Base, this);
	
	if(IsValid(Native))
	{

#if WITH_EDITOR
		Native->SetActorLabel(FString::Printf(TEXT("%s - %s"), *Base->SpeckleType, *Base->Id));
#endif
		
        Native->AttachToActor(AOwner, FAttachmentTransformRules::KeepRelativeTransform);
        Native->SetOwner(AOwner);

        InProgressObjectsCache.Add(Native);
	}
	else
	{
		Native = AOwner;
	}
	
	//Convert Children
	TMap<FString, TSharedPtr<FJsonValue>> PotentialChildren = IsValid(Base)? Base->DynamicProperties : SpeckleObject->Values;
	
	for (const auto& Kv : PotentialChildren)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Kv.Value->TryGetObject(SubObjectPtr))
		{
			ImportObjectFromCache(Native, *SubObjectPtr);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (const auto ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (!ArrayElement->TryGetObject(ArraySubObjPtr))
					continue;
				ImportObjectFromCache(Native, *ArraySubObjPtr);
			}
		}
	}
}


UBase* ASpeckleUnrealManager::DeserializeBase(const TSharedPtr<FJsonObject> Obj) const
{
	check(Obj != nullptr);
	
	{ // Handle Detached Objects
		TSharedPtr<FJsonObject> DetachedObject;
		if(ResolveReference(Obj, DetachedObject))
		{
			return DeserializeBase(DetachedObject);
		}
	}
	
	FString SpeckleType;	
	if (!Obj->TryGetStringField("speckle_type", SpeckleType)) return nullptr;
	FString ObjectId = "";	
	Obj->TryGetStringField("id", ObjectId);
		
	TSubclassOf<UBase> BaseType = UBase::FindClosestType(SpeckleType);
	
	if(BaseType == nullptr)
	{
		UE_LOG(LogSpeckle, Verbose, TEXT("Skipping deserialization of %s %s: Unrecognised SpeckleType"), *SpeckleType, *ObjectId );
		BaseType = UBase::StaticClass();
	}
	
	UBase* Base =  NewObject<UBase>(GetTransientPackage(), BaseType);
	if(Base->Parse(Obj, this)) return Base;
	
	UE_LOG(LogSpeckle, Verbose, TEXT("Skipping deserialization of %s %s: Object could not be deserialised to closest type %s"), *SpeckleType, *ObjectId, *BaseType->GetName());
	return nullptr;
}

bool ASpeckleUnrealManager::TryParseSpeckleObjectFromJsonProperty(const TSharedPtr<FJsonValue> JsonValue, UBase*& OutBase) const
{
	check(JsonValue != nullptr);
	
	const TSharedPtr<FJsonObject>* JsonObjectPtr;
	if(JsonValue->TryGetObject(JsonObjectPtr))
	{
		TSharedPtr<FJsonObject> JsonObject;
		if(!ResolveReference(*JsonObjectPtr, JsonObject))
			JsonObject = *JsonObjectPtr;

		if(JsonObject.IsValid())
		{
			//Handle Speckle object types
			OutBase = DeserializeBase(JsonObject);
			return IsValid(OutBase);
		}		
	}
	return false;
}

bool ASpeckleUnrealManager::HasObject(const FString& Id) const
{
	return SpeckleObjects.Contains(Id);
}

TSharedPtr<FJsonObject, ESPMode::Fast> ASpeckleUnrealManager::GetSpeckleObject(const FString& Id) const
{
	return *SpeckleObjects.Find(Id);
}

bool ASpeckleUnrealManager::ResolveReference(const TSharedPtr<FJsonObject> Object, TSharedPtr<FJsonObject>& OutObject) const
{
	FString SpeckleType;	
	FString ReferenceID;
	
	if (Object->TryGetStringField("speckle_type", SpeckleType)
		&& SpeckleType == "reference"
		&& Object->TryGetStringField("referencedId",ReferenceID))
	{
		OutObject = GetSpeckleObject(ReferenceID);
		return true;
	}
	return false;
}


TArray<TSharedPtr<FJsonValue>> ASpeckleUnrealManager::CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField) const
{
	TArray<TSharedPtr<FJsonValue>> ObjectPoints;
		
	for(int32 i = 0; i < ArrayField.Num(); i++) 
	{
		FString Index;
		if(ArrayField[i]->AsObject()->TryGetStringField("referencedId", Index))
		{
			const auto Chunk = SpeckleObjects[Index]->GetArrayField("data");
			ObjectPoints.Append(Chunk);
		}
		else
		{
			return ArrayField; //Array was never chunked to begin with
		}
	}
	return ObjectPoints;
}

float ASpeckleUnrealManager::ParseScaleFactor(const FString& Units) const
{
	static const auto ParseUnits = [](const FString& LUnits) -> float
	{
		if (LUnits == "millimeters" || LUnits == "millimeter" || LUnits == "millimetres" || LUnits == "millimetre" || LUnits == "mm")
			return 0.1;
		if (LUnits == "centimeters" || LUnits == "centimeter" ||LUnits == "centimetres" || LUnits == "centimetre" || LUnits == "cm")
			return 1;
		if (LUnits == "meters" || LUnits == "meter" || LUnits == "metres" || LUnits == "metre" || LUnits == "m")
			return 100;
		if (LUnits == "kilometers" || LUnits == "kilometres" || LUnits == "km")
			return 100000;

		if (LUnits == "inches" || LUnits == "inch" || LUnits == "in")
			return 2.54;
		if (LUnits == "feet" || LUnits == "foot" || LUnits == "ft")
			return 30.48;
		if (LUnits == "yards" || LUnits == "yard"|| LUnits == "yd")
			return 91.44;
		if (LUnits == "miles" || LUnits == "mile" || LUnits == "mi")
			return 160934.4;

		return 100;
	};

	return ParseUnits(Units.ToLower()) * WorldToCentimeters;
}




