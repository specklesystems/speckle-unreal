#include "JsonObjectConverter.h"
#include "SpeckleUnrealActor.h"
#include "SpeckleUnrealManager.h"
#include "Objects/Mesh.h"

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
	if(Base == nullptr)
		return;
		
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
	for (const auto& Kv : SpeckleObject->Values)
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

bool ASpeckleUnrealManager::TryGetMaterial(const URenderMaterial* SpeckleMaterial, const bool AcceptMaterialOverride, UMaterialInterface*& OutMaterial)
{
	const auto MaterialID = SpeckleMaterial->Id;
	
	if(AcceptMaterialOverride)
	{
		//Override by id
		if(MaterialOverridesById.Contains(MaterialID))
		{
			OutMaterial = MaterialOverridesById[MaterialID];
			return true;
		}
		//Override by name
		const FString Name = SpeckleMaterial->Name;
		for (const UMaterialInterface* Mat : MaterialOverridesByName)
		{
			if(Mat->GetName() == Name)
			{
				OutMaterial = MaterialOverridesById[MaterialID];
				return true;
			}
		}
	}

		
	if(ConvertedMaterials.Contains(MaterialID))
	{
		OutMaterial = ConvertedMaterials[MaterialID];
		return true;
	}

	return false;
}

UBase* ASpeckleUnrealManager::DeserializeBase(const TSharedPtr<FJsonObject> Obj) const
{	
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

	// Get the registered  type from Base register
	const TSubclassOf<UBase> ObjectType = UBase::FindClosestType(SpeckleType);
	
	if(ObjectType == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SpeckleType: %s is unknown,%t object: %s will be ignored"), *SpeckleType, *ObjectId );
		return nullptr; //BaseType = UBase::StaticClass();
	}


	//TODO before we create and deserialised a new object, first check if we have already deserialised this object
	
	//Create instance of the ObjectType
	UBase* Base = NewObject<UBase>(GetTransientPackage(), ObjectType);
		
	//Map of all properties with no explicit UProperty to set.
	//For now we add all values to this map, then remove the ones we find explicit UProperties for.
	auto DynamicProperties = TMap<FString, TSharedPtr<FJsonValue>>(Obj->Values);
	
	//Loop through each UProperty in the UBase and try and set its value from the JSON obj
	for (TFieldIterator<UProperty> It(ObjectType); It; ++It)
	{
		FProperty* Property = *It;
		void* PropertyValueAddress = Property->ContainerPtrToValuePtr<uint8>(Base);

		// Find a json value matching this property name
		const FString Key = Property->GetName();
		const TSharedPtr<FJsonValue>* JsonValuePtr = Obj->Values.Find(Key);
		
		
		//Ensure value is valid
		if (!JsonValuePtr) continue;
		TSharedPtr<FJsonValue> JsonValue = *JsonValuePtr;
		if ( (!JsonValue.IsValid()) || JsonValue->IsNull() ) continue;
		
		//Handle chunked values!!!!!
		if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			const TSharedPtr<FJsonObject>* ChunkedObject;
			FString ChunkedType;
			if(JsonValue->TryGetObject(ChunkedObject)
				&& ChunkedObject->operator->()->TryGetStringField("", ChunkedType)
				&& ChunkedType == "Speckle.Core.Models.DataChunk")
			{
				const TArray<TSharedPtr<FJsonValue>>* ChunkedArray;
				if(JsonValue->TryGetArray(ChunkedArray))
				{
					auto Arr = CombineChunks(*ChunkedArray);
					if()
					{
						
					}
				}
			}
		}
		
		//Check if property is a Speckle Object
		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			UBase* SpeckleObject;
			if(TryParseSpeckleObjectFromJsonProperty(JsonValue, SpeckleObject))
			{
				ObjectProperty->SetObjectPropertyValue(PropertyValueAddress, SpeckleObject);
				DynamicProperties.Remove(Key);
				continue;
			}
		}
			
		//Handle primitive types
		if(FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, PropertyValueAddress, 0, 0))
		{
			DynamicProperties.Remove(Key);
			continue;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to deserialise a value, object id: %s, property key: %s"), *ObjectId, *Key)
		}
	}

	//Find any remaining Speckle objects and add them as children
	TMap<FString, UBase*> ChildBases;
	{
		const auto DynamicPropertiesCopy = TMap<FString, TSharedPtr<FJsonValue>>(DynamicProperties);
		for(const auto& Kvp : DynamicPropertiesCopy)
		{
			UBase* ChildObject;
			if(TryParseSpeckleObjectFromJsonProperty(Kvp.Value, ChildObject))
			{
				DynamicProperties.Remove(Kvp.Key);
				ChildBases.Add(ChildObject->Id, ChildObject);
			}		
		}
	}
	
	Base->DynamicProperties = DynamicProperties;
	Base->Children = ChildBases;
	return Base;
}












bool ASpeckleUnrealManager::TryParseSpeckleObjectFromJsonProperty(const TSharedPtr<FJsonValue> JsonValue, UBase*& OutBase) const
{
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


void Test(const TSharedPtr<FJsonValue> JsonValue, const FProperty* Property, UObject* Base)
{
	if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
	{
		if (NumericProperty->IsFloatingPoint())
		{
			NumericProperty->SetFloatingPointPropertyValue(Base, JsonValue->AsNumber());
		}
		else if (NumericProperty->IsInteger())
		{
			NumericProperty->SetIntPropertyValue(Base, (int64)JsonValue->AsNumber());
		}
	}
	else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		// Export bools as bools
		BoolProperty->SetPropertyValue(Base, JsonValue->AsBool());
	}
	else if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		StringProperty->SetPropertyValue(Base, JsonValue->AsString());
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
				
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
				
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		if (JsonValue->Type != EJson::Array) return;

		const TArray< TSharedPtr<FJsonValue> > ArrayValue = JsonValue->AsArray();
		int32 ArrLen = ArrayValue.Num();

		// make the output array size match
		
	}
	else if (const FStructProperty *StructProperty = CastField<FStructProperty>(Property))
	{
				
	}
	else if (const FObjectProperty *ObjectProperty = CastField<FObjectProperty>(Property))
	{
				
	}
	else
	{
				
	}
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


AActor* ASpeckleUnrealManager::CreateBlockInstance(const TSharedPtr<FJsonObject> Obj)
{
	//Transform
    const FString Units = Obj->GetStringField("units");
	const float ScaleFactor = ParseScaleFactor(Units);
	
	const TArray<TSharedPtr<FJsonValue>>* TransformData;
	if(!Obj->TryGetArrayField("transform", TransformData)) return nullptr;
	
	
	FMatrix TransformMatrix;
	for(int32 Row = 0; Row < 4; Row++)
	for(int32 Col = 0; Col < 4; Col++)
	{
		TransformMatrix.M[Row][Col] = TransformData->operator[](Row * 4 + Col)->AsNumber();
	}
	TransformMatrix = TransformMatrix.GetTransposed();
	TransformMatrix.ScaleTranslation(FVector(ScaleFactor));
	
	//Block Instance
	const FString ObjectId = Obj->GetStringField("id"), SpeckleType = Obj->GetStringField("speckle_type");

	ASpeckleUnrealActor* BlockInstance = World->SpawnActor<ASpeckleUnrealActor>(ASpeckleUnrealActor::StaticClass(), FTransform(TransformMatrix));

	
	//Block Definition
	const TSharedPtr<FJsonObject>* BlockDefinitionReference;
	if(!Obj->TryGetObjectField("blockDefinition", BlockDefinitionReference)) return nullptr;
	
	const FString RefID = BlockDefinitionReference->operator->()->GetStringField("referencedId");
	
	const TSharedPtr<FJsonObject> BlockDefinition = SpeckleObjects[RefID];
	
	//For now just recreate mesh, eventually we should use instanced static mesh
	const auto Geometries = BlockDefinition->GetArrayField("geometry");

	for(const auto Child : Geometries)
	{
		const TSharedPtr<FJsonObject> MeshReference = Child->AsObject();
		const FString MeshID = MeshReference->GetStringField("referencedId");
		
		//It is important that ParentObject is the BlockInstance not the BlockDefinition to keep NativeIDs of meshes unique between Block Instances
		ImportObjectFromCache(BlockInstance, SpeckleObjects[MeshID]);
	}
	
	
	return BlockInstance;
}



