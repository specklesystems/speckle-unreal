#include "SpeckleUnrealManager.h"

#include "MaterialConverter.h"



bool ASpeckleUnrealManager::TryGetExistingNative(const FString &ObjectId, UObject*& OutObject)
{
	if (InProgressObjects.Contains(ObjectId))
	{
		OutObject = InProgressObjects[ObjectId];
		return true;
	}
	
	//if(!CreatedObjects.Contains(ObjectId))
		return false;
	//
	//OutObject = CreatedObjects[ObjectId];
	//return OutObject->IsValidLowLevelFast();
}


void ASpeckleUnrealManager::ImportObjectFromCache(const TSharedPtr<FJsonObject> SpeckleObj, const TSharedPtr<FJsonObject> ParentObj)
{
	if (!SpeckleObj->HasField("speckle_type"))
		return;
	if (SpeckleObj->GetStringField("speckle_type") == "reference" && SpeckleObj->HasField("referencedId")) {
		TSharedPtr<FJsonObject> ReferencedObj;
		if (SpeckleObjects.Contains(SpeckleObj->GetStringField("referencedId")))
			ImportObjectFromCache(SpeckleObjects[SpeckleObj->GetStringField("referencedId")], ParentObj);		
		return;
	}
	if (!SpeckleObj->HasField("id"))
		return;

	
	const FString ObjectId = SpeckleObj->GetStringField("id");
	const FString SpeckleType = SpeckleObj->GetStringField("speckle_type");
	
	UObject* Native;
	if(SpeckleType == "Objects.Geometry.Mesh")
	{
		if(!TryGetExistingNative(ObjectId, Native))
		{
			Native = CreateMesh(SpeckleObj, ParentObj);
		}
	}
	//else if(SpeckleType == "Objects.Geometry.PointCloud")
	//{
	//	if(!TryGetExistingNative(ObjectId, Native))
	//	{
	//		Native = CreatePointCloud(SpeckleObj);
	//	}
	//}
	else
	{
		Native = nullptr ;
	}
	
	if(Native)
	{
		InProgressObjects.Add(ObjectId, Native);
	}

	//Object has no native conversion, go through each of its properties and try convert that

	// Go recursively into all object fields (except @displayMesh)
	for (const auto& Kv : SpeckleObj->Values)
	{

		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Kv.Value->TryGetObject(SubObjectPtr))
		{
			ImportObjectFromCache(*SubObjectPtr, SpeckleObj);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (auto& ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (!ArrayElement->TryGetObject(ArraySubObjPtr))
					continue;
				ImportObjectFromCache(*ArraySubObjPtr, SpeckleObj);
			}
		}
	}
}

UMaterialInterface* ASpeckleUnrealManager::CreateMaterial(TSharedPtr<FJsonObject> RenderMaterialObject, const bool AcceptMaterialOverride)
{
	if (RenderMaterialObject->GetStringField("speckle_type") == "reference")
		RenderMaterialObject = SpeckleObjects[RenderMaterialObject->GetStringField("referencedId")];

	//Parse to a URenderMaterial
	const URenderMaterial* SpeckleMaterial = UMaterialConverter::ParseRenderMaterial(RenderMaterialObject);

	return CreateMaterial(SpeckleMaterial, AcceptMaterialOverride);
}

UMaterialInterface* ASpeckleUnrealManager::CreateMaterial(const URenderMaterial* SpeckleMaterial, const bool AcceptMaterialOverride)
{
	const auto MaterialID = SpeckleMaterial->ObjectID;

	
	if(AcceptMaterialOverride)
	{
		//Override by id
		if(MaterialOverridesById.Contains(MaterialID))
		{
			return MaterialOverridesById[MaterialID];
		}
		//Override by name
		const FString Name = SpeckleMaterial->Name;
		for (UMaterialInterface* Mat : MaterialOverridesByName)
		{
			if(Mat->GetName() == Name) return Mat;
		}
	}


	if(ConvertedMaterials.Contains(MaterialID))
	{
		return ConvertedMaterials[MaterialID];
	}
	
	//Create Convert Material Instance
	UMaterialInterface* ExplicitMaterial;
	if(SpeckleMaterial->Opacity >= 1)
		ExplicitMaterial = BaseMeshOpaqueMaterial;
	else
		ExplicitMaterial = BaseMeshTransparentMaterial;
		
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(ExplicitMaterial, this, FName(SpeckleMaterial->Name));
	UMaterialConverter::AssignPropertiesFromSpeckle(DynMaterial, SpeckleMaterial);

	ConvertedMaterials.Add(MaterialID, DynMaterial);
	
	return DynMaterial;
}



TArray<TSharedPtr<FJsonValue>> ASpeckleUnrealManager::CombineChunks(const TArray<TSharedPtr<FJsonValue>>* ArrayField)
{
	TArray<TSharedPtr<FJsonValue>> ObjectPoints;
	for(int i = 0; i < ArrayField->Num(); i++)
	{
		const FString Index = ArrayField->operator[](i)->AsObject()->GetStringField("referencedId");
		const auto Chunk = SpeckleObjects[Index]->GetArrayField("data");
		ObjectPoints.Append(Chunk);
	}
	return ObjectPoints;
}

float ASpeckleUnrealManager::ParseScaleFactor(const FString& Units)
{
	// unreal engine units are in cm by default but the conversion is editable by users so
	// this needs to be accounted for later.
	const FString LUnits = Units.ToLower();
	if (LUnits == "meters" || LUnits == "metres" || LUnits == "m")
		return 100;

	if (LUnits == "centimeters" || LUnits == "centimetres" || LUnits == "cm")
		return 1;

	if (LUnits == "millimeters" || LUnits == "millimetres" || LUnits == "mm")
		return 0.1;

	if (LUnits == "yards" || LUnits == "yd")
		return 91.4402757;

	if (LUnits == "feet" || LUnits == "ft")
		return 30.4799990;

	if (LUnits == "inches" || LUnits == "in")
		return 2.5399986;

	return 1;
}


ASpeckleUnrealMesh* ASpeckleUnrealManager::CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent)
{
	const FString ObjId = Obj->GetStringField("id");
	UE_LOG(LogTemp, Log, TEXT("Creating mesh for object %s"), *ObjId);

	const FString Units = Obj->GetStringField("units");
	// unreal engine units are in cm by default but the conversion is editable by users so
	// this needs to be accounted for later.
	
	const float ScaleFactor = ParseScaleFactor(Units);

	// The following line can be used to debug large objects
	// ScaleFactor = ScaleFactor * 0.1;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ASpeckleUnrealMesh* MeshInstance = World->SpawnActor<ASpeckleUnrealMesh>(MeshActor, SpawnParams);

	// attaches each speckleMesh under this actor (SpeckleManager)
	MeshInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	MeshInstance->SetOwner(this);

	MeshInstance->SetActorLabel(FString::Printf(TEXT("%s - %s"), *ASpeckleUnrealMesh::StaticClass()->GetName(), *ObjId));


	//Parse Vertices
	TArray<FVector> ParsedVertices;
	{
		TArray<TSharedPtr<FJsonValue>> ObjectVertices = CombineChunks(&Obj->GetArrayField("vertices"));
		const auto NumberOfVertices = ObjectVertices.Num() / 3;
	
		ParsedVertices.SetNum(NumberOfVertices);

		for (size_t i = 0, j = 0; i < NumberOfVertices; i++, j += 3)
		{
			ParsedVertices[i] = FVector
			(
				ObjectVertices[j].Get()->AsNumber() * -1,
				ObjectVertices[j + 1].Get()->AsNumber(),
				ObjectVertices[j + 2].Get()->AsNumber()
			) * ScaleFactor;
		
		}
	}



	//Parse Triangles
	TArray<int32> ParsedTriangles;
	{
		TArray<TSharedPtr<FJsonValue>> ObjectFaces = CombineChunks(&Obj->GetArrayField("faces"));
		//convert mesh faces into triangle array regardless of whether or not they are quads

		int32 j = 0;
		while (j < ObjectFaces.Num())
		{
			if (ObjectFaces[j].Get()->AsNumber() == 0)
			{
				//Triangles
				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
				j += 4;
			}
			else
			{
				//Quads to triangles
				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());

				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 4].Get()->AsNumber());

				j += 5;
			}
		}
	}


	// Material priority (low to high): DefaultMeshMaterial, Material set on parent, Converted RenderMaterial set on mesh, MaterialOverridesByName match, MaterialOverridesById match
	UMaterialInterface* Material;

	if (Obj->HasField("renderMaterial"))
	{
		Material = CreateMaterial(Obj->GetObjectField("renderMaterial"));
	}
	else if (Parent && Parent->HasField("renderMaterial"))
	{
		Material = CreateMaterial(Parent->GetObjectField("renderMaterial"));
	}
	else
		Material = DefaultMeshMaterial;

	MeshInstance->SetMesh(ParsedVertices, ParsedTriangles, Material, FLinearColor::White);

	//UE_LOG(LogTemp, Warning, TEXT("Added %d vertices and %d triangles"), ParsedVertices.Num(), ParsedTriangles.Num());

	return MeshInstance;
}
