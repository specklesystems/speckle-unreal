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
		
	const TSubclassOf<UBase> BaseType = UBase::FindClosestType(SpeckleType);
	
	if(BaseType == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SpeckleType: %s is unknown,%t object: %s will be ignored"), *SpeckleType, *ObjectId );
		return nullptr; //BaseType = UBase::StaticClass();
	}

	
	UBase* Base =  NewObject<UBase>(GetTransientPackage(), BaseType);
	Base->Parse(Obj, this);

	return Base;
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



