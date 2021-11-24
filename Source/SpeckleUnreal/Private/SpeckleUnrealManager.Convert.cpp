#include "SpeckleUnrealManager.h"
#include "Objects/Mesh.h"
#include "Objects/PointCloud.h"

#include "Objects/RenderMaterial.h"
#include "Panagiotis/SpeckleRESTHandlerComponent.h"



		// --  Print as String --
		// FString OutputString;
		// TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		// FJsonSerializer::Serialize(SpeckleObject.ToSharedRef(), Writer);
		// UE_LOG(LogTemp, Warning, TEXT("resulting jsonString -> %s"), *OutputString);
		// -- -------------------- --
		
		// //--------------
		// FString LayerName = "";
		//
		// // Iterate over Json Values
		// if (ParentObject.IsValid())
		// {
		// 	for (auto currJsonValue = ParentObject->Values.CreateConstIterator(); currJsonValue; ++currJsonValue)
		// 	{
		// 		// Get the key name
		// 		FString KeyName = (*currJsonValue).Key;
		// 		// TSharedPtr<FJsonValue> KeyValue = (*currJsonValue).Value;
		//
		//
		// 		// FString OutputString2;
		// 		// TSharedRef< TJsonWriter<> > Writer2 = TJsonWriterFactory<>::Create(&OutputString2);
		// 		// TSharedRef<FJsonValue, ESPMode::Fast> k = KeyValue.ToSharedRef();
		//
		// 		
		// 			// FJsonSerializer::Serialize(KeyValue->AsArray(), Writer2);
		// 			// UE_LOG(LogTemp, Warning, TEXT("resulting jsonString2a -> %s"), *OutputString2);
		// 		 //
		// 		 //    double d = KeyValue->AsNumber();
		// 			// UE_LOG(LogTemp, Warning, TEXT("resulting jsonString2b -> %5.2f"), d);
		// 			//
		// 			// FString s = KeyValue->AsString();
		// 			// UE_LOG(LogTemp, Warning, TEXT("resulting jsonString2c -> %s"), *s);
		// 			
		// 		if (KeyName.Contains("::"))
		// 		{
		// 			UE_LOG(LogTemp, Warning, TEXT("Key Name -> %s"), *KeyName);
		// 			
		// 			LayerName = KeyName; // FString::Printf(TEXT("%s"), *KeyName ) ;
		//
		// 			UE_LOG(LogTemp, Warning, TEXT("LayerName Name -> %s"), *LayerName);
		// 			
		// 			
		// 			// for (int i=0; i < kv_Array.Num(); i++)
		// 			// {
		// 			// 	FString OutputString = kv_Array[i]->AsString();
		// 			// 	//TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		// 			// 	
		// 			// 	UE_LOG(LogTemp, Warning, TEXT("Key Value Data -> %s"), *OutputString);
		// 			// }
		// 			
		// 		}
		// 		// Get the value as a FJsonValue object
		// 		//TSharedPtr< FJsonValue > Value = (*currJsonValue).Value;
		// 	}
		// }
		// UE_LOG(LogTemp, Warning, TEXT("LayerName Name -> %s"), *LayerName);
		//----------------------


TMap<FString,FString> ASpeckleUnrealManager::ImportObjectFromCacheNew(AActor* AOwner,
													 const TSharedPtr<FJsonObject> SpeckleObject,
                                                     const TSharedPtr<FJsonObject> ParentObject,
                                                     TMap<FString, FString> ObjectsMap,
                                                     FString Who)
{
	//UE_LOG(LogTemp, Warning, TEXT("EXECUTING Map size %d %s"), ObjectsMap.Num(), *Who );


	
	for (auto currJsonValue = SpeckleObject->Values.CreateConstIterator(); currJsonValue; ++currJsonValue)
	{
		// Get the key name
		FString KeyName = (*currJsonValue).Key;
		TSharedPtr<FJsonValue, ESPMode::Fast> KeyValue = (*currJsonValue).Value;

		if (KeyName.Contains("::"))
		{
            FString id_Layer="";

			

			const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
			if (KeyValue->TryGetArray(SubArrayPtr))
			{
				for (const auto& ArrayElement : *SubArrayPtr)
				{
					const TSharedPtr<FJsonObject>* ArraySubObjPtr;

					if (!ArrayElement->TryGetObject(ArraySubObjPtr))
						continue;
					else
					{
						
						TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&id_Layer);
						FJsonSerializer::Serialize((*ArraySubObjPtr).ToSharedRef(), Writer);
					}
				}
			}
			
			//UE_LOG(LogTemp, Warning, TEXT("resulting jsonString -> %s"), *OutputString);

			UE_LOG(LogTemp, Warning, TEXT("KEY VAL 2: %s - %s"), *KeyName, *currJsonValue.Key());
			ObjectsMap.Add(*KeyName, *id_Layer);
		}
	}
	
	// If it doesn't have type then ignore
	if (!SpeckleObject->HasField("speckle_type"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Speckle Type"));
		return ObjectsMap;
	}

	// Check if it is a reference
	if (SpeckleObject->GetStringField("speckle_type") == "reference" && SpeckleObject->HasField("referencedId"))
	{
		TSharedPtr<FJsonObject> ReferencedObj;
		if (SpeckleObjects.Contains(SpeckleObject->GetStringField("referencedId")))
		{
			ImportObjectFromCacheNew(AOwner, SpeckleObjects[SpeckleObject->GetStringField("referencedId")],
			                         ParentObject, ObjectsMap, "REFERENCE");
			
			//UE_LOG(LogTemp, Warning, TEXT("It is a reference"));
		}
		return ObjectsMap;
	}

	// If it does not have id, then return
	if (!SpeckleObject->HasField("id"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("No id"));
		return ObjectsMap;
	}


	const FString ObjectId = SpeckleObject->GetStringField("id");
	const FString SpeckleType = SpeckleObject->GetStringField("speckle_type");

	AActor* Native = nullptr;


	if (SpeckleType == "Objects.Geometry.Mesh")
	{
		Native = CreateMesh(SpeckleObject, ParentObject);
	}
	else if (SpeckleType == "Objects.Geometry.Pointcloud")
	{
		Native = CreatePointCloud(SpeckleObject);
	}
	else if (SpeckleType == "Objects.Other.BlockInstance")
	{
		Native = CreateBlockInstance(SpeckleObject);
	}
	else if (SpeckleType == "Objects.Other.BlockDefinition")
	{
		return ObjectsMap; //Ignore block definitions, Block instances will create geometry instead.
	}


	if (IsValid(Native))
	{
#if WITH_EDITOR
		//UE_LOG(LogTemp, Warning, TEXT("%s - %s - %s"), *LayerName, *SpeckleType, *ObjectId);
		Native->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjectId));
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
			ImportObjectFromCacheNew(Native, *SubObjectPtr, SpeckleObject, ObjectsMap, "ChildObject");
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (const auto& ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;

				if (!ArrayElement->TryGetObject(ArraySubObjPtr))
					continue;

				ImportObjectFromCacheNew(Native, *ArraySubObjPtr, SpeckleObject, ObjectsMap, "ChildArray");
			}
		}
	}
	return ObjectsMap;
}






void ASpeckleUnrealManager::ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject,
                                                                  const TSharedPtr<FJsonObject> ParentObject)
{

	// If it doesn't have type then ignore
	if (!SpeckleObject->HasField("speckle_type"))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Speckle Type"));
		return;
	}

	// Check if it is a reference
	if (SpeckleObject->GetStringField("speckle_type") == "reference" && SpeckleObject->HasField("referencedId")) {
		TSharedPtr<FJsonObject> ReferencedObj;
		if (SpeckleObjects.Contains(SpeckleObject->GetStringField("referencedId")))
		{
			ImportObjectFromCache(AOwner, SpeckleObjects[SpeckleObject->GetStringField("referencedId")], ParentObject);
			UE_LOG(LogTemp, Warning, TEXT("It is a reference"));
		}
		return;
	}

	// If it does not have id, then return
	if (!SpeckleObject->HasField("id"))
	{
		UE_LOG(LogTemp, Warning, TEXT("No id"));
		return;
	}

	
	const FString ObjectId    = SpeckleObject->GetStringField("id");
	const FString SpeckleType = SpeckleObject->GetStringField("speckle_type");
	
	AActor* Native = nullptr;

	
	if(SpeckleType == "Objects.Geometry.Mesh")
	{
		Native = CreateMesh(SpeckleObject, ParentObject);
	}
	else if(SpeckleType == "Objects.Geometry.Pointcloud")
	{
	    Native = CreatePointCloud(SpeckleObject);
	}
	else if(SpeckleType == "Objects.Other.BlockInstance")
	{
		Native = CreateBlockInstance(SpeckleObject);
	}
	else if(SpeckleType == "Objects.Other.BlockDefinition")
	{
		return; //Ignore block definitions, Block instances will create geometry instead.
	}

	
	if(IsValid(Native))
	{

#if WITH_EDITOR
		//UE_LOG(LogTemp, Warning, TEXT("%s - %s - %s"), *LayerName, *SpeckleType, *ObjectId);
		Native->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjectId));
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
			ImportObjectFromCache(Native, *SubObjectPtr, SpeckleObject);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (const auto& ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (!ArrayElement->TryGetObject(ArraySubObjPtr))
					continue;
				ImportObjectFromCache(Native, *ArraySubObjPtr, SpeckleObject);
			}
		}
	}
}







bool ASpeckleUnrealManager::TryGetMaterial(const URenderMaterial* SpeckleMaterial, const bool AcceptMaterialOverride,
																	UMaterialInterface*& OutMaterial)
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


ASpeckleUnrealActor* ASpeckleUnrealManager::CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent)
{
	const FString ObjId = Obj->GetStringField("id");

	
	UMesh* Mesh = NewObject<UMesh>();
	Mesh->Parse(Obj, this);
		
	ASpeckleUnrealActor* ActorInstance = World->SpawnActor<ASpeckleUnrealActor>(MeshActor, FTransform(Mesh->Transform));

	

	// Material priority (low to high): DefaultMeshMaterial, Material set on parent, Converted RenderMaterial set on mesh, MaterialOverridesByName match, MaterialOverridesById match
	URenderMaterial* Material = NewObject<URenderMaterial>();

	if (Obj->HasField("renderMaterial"))
	{
		Material->Parse(Obj->GetObjectField("renderMaterial"), this);
	}
	else if (Parent && Parent->HasField("renderMaterial"))
	{
		Material->Parse(Parent->GetObjectField("renderMaterial"), this);
	}
	
	Mesh->RenderMaterial = Material;

	if(ActorInstance->GetClass()->ImplementsInterface(USpeckleMesh::StaticClass()))
	{
		FEditorScriptExecutionGuard ScriptGuard;
		ISpeckleMesh::Execute_SetMesh(ActorInstance, Mesh, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not implement $s interface"), MeshActor , USpeckleMesh::StaticClass());
	}
	
	return ActorInstance;
}

AActor* ASpeckleUnrealManager::CreatePointCloud(const TSharedPtr<FJsonObject> Obj)
{
	const FString ObjId = Obj->GetStringField("id");
	UE_LOG(LogTemp, Log, TEXT("Creating PointCloud for object %s"), *ObjId);
	
	
	UPointCloud* Base = NewObject<UPointCloud>();
	Base->Parse(Obj, this);
		
	AActor* ActorInstance = World->SpawnActor<AActor>(PointCloudActor);
	
	if(ActorInstance->GetClass()->ImplementsInterface(USpecklePointCloud::StaticClass()))
	{
		FEditorScriptExecutionGuard ScriptGuard;
		ISpecklePointCloud::Execute_SetData(ActorInstance, Base, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not implement $s interface"), PointCloudActor , USpecklePointCloud::StaticClass());
	}
	
	return ActorInstance;
}



ASpeckleUnrealActor* ASpeckleUnrealManager::CreateBlockInstance(const TSharedPtr<FJsonObject> Obj)
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
		ImportObjectFromCache(BlockInstance, SpeckleObjects[MeshID], Obj);
	}
	
	
	return BlockInstance;
}



