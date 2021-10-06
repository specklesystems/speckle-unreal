#include "SpeckleUnrealManager.h"

#include "MaterialConverter.h"



bool ASpeckleUnrealManager::TryGetExistingNative(const FString &ObjectId, UObject*& OutObject)
{
	//if (InProgressObjectsCache.Contains(ObjectId))
	//{
	//	OutObject = InProgressObjectsCache[ObjectId];
	//	return true;
	//}
	
	//if(!CreatedObjects.Contains(ObjectId))
		return false;
	//
	//OutObject = CreatedObjects[ObjectId];
	//return OutObject->IsValidLowLevelFast();
}


void ASpeckleUnrealManager::ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject, const TSharedPtr<FJsonObject> ParentObject)
{
	if (!SpeckleObject->HasField("speckle_type"))
		return;
	if (SpeckleObject->GetStringField("speckle_type") == "reference" && SpeckleObject->HasField("referencedId")) {
		TSharedPtr<FJsonObject> ReferencedObj;
		if (SpeckleObjects.Contains(SpeckleObject->GetStringField("referencedId")))
			ImportObjectFromCache(AOwner, SpeckleObjects[SpeckleObject->GetStringField("referencedId")], ParentObject);
		return;
	}
	if (!SpeckleObject->HasField("id"))
		return;

	
	const FString ObjectId = SpeckleObject->GetStringField("id");
	const FString SpeckleType = SpeckleObject->GetStringField("speckle_type");

	//Owner's id is included in identifier to allow for BlockInstances sharing meshes with the same ID
	//const ASpeckleUnrealActor* SOwner = dynamic_cast<ASpeckleUnrealActor*>(AOwner);
	//FString NativeId = SOwner? FString::Printf(TEXT("%s-%s"), *SOwner->NativeID, *ObjectId) : ObjectId;

	//if(SOwner)
	//	UE_LOG(LogTemp, Warning, TEXT("Parent ID = %s"), *SOwner->NativeID);
	
	AActor* Native = nullptr;

	//if( !(CreatedObjectsCache.Contains(NativeId)
	//	&& IsValid(Native = dynamic_cast<AActor*>(CreatedObjectsCache[NativeId]))) )
	{
		if(SpeckleType == "Objects.Geometry.Mesh")
		{
			Native = CreateMesh(SpeckleObject, ParentObject);
		}
		//else if(SpeckleType == "Objects.Geometry.PointCloud")
		//{
		//    Native = CreatePointCloud(SpeckleObj);
		//}
		else if(SpeckleType == "Objects.Other.BlockInstance")
		{
			Native = CreateBlockInstance(SpeckleObject);
		}
		else if(SpeckleType == "Objects.Other.BlockDefinition")
		{
			return; //Ignore block definitions, Block instances will create geometry instead.
		}
	}
	

	//if(!IsValid(Native)) //TODO disabled because it creates many empty objects
	//{
	//	//Create Empty actor (to preserve Hierarchy)
	//	Native = World->SpawnActor<AActor>();
	//	Native->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjectId));
	//}

	if(IsValid(Native))
	{
		//if(InProgressObjectsCache.Contains(NativeId))
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Native ID {%s} allready exists in cache, object will be destroyed"), *NativeId);
		//	Native->Destroy();
		//	Native = AOwner;
		//}
		//else
		{
		//	ASpeckleUnrealActor* SNative = dynamic_cast<ASpeckleUnrealActor*>(Native);
		//	if(SNative)
		//	{
		//		SNative->NativeID = NativeId;
		//		UE_LOG(LogTemp, Warning, TEXT("Native ID = %s"), *SNative->NativeID);
		//	}

			//if(!AOwner->HasValidRootComponent()) AOwner->SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
			
			Native->AttachToActor(AOwner, FAttachmentTransformRules::KeepRelativeTransform);
			Native->SetOwner(AOwner);

			InProgressObjectsCache.Add(Native);
		//	InProgressObjectsCache.Add(NativeId, Native);
		}
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



TArray<TSharedPtr<FJsonValue>> ASpeckleUnrealManager::CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField)
{
	TArray<TSharedPtr<FJsonValue>> ObjectPoints;
	for(int i = 0; i < ArrayField.Num(); i++)
	{
		const FString Index = ArrayField[i]->AsObject()->GetStringField("referencedId");
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
	
	if (LUnits == "kilometers" || LUnits == "kilometres" || LUnits == "km")
		return 100000;
	if (LUnits == "meters" || LUnits == "meter" || LUnits == "metres" || LUnits == "metre" || LUnits == "m")
		return 100;
	if (LUnits == "centimeters" || LUnits == "centimeter" ||LUnits == "centimetres" || LUnits == "centimetre" || LUnits == "cm")
		return 1;
	if (LUnits == "millimeters" || LUnits == "millimeter" || LUnits == "millimetres" || LUnits == "millimetre" || LUnits == "mm")
		return 0.1;

	if (LUnits == "miles" || LUnits == "mile" || LUnits == "mi")
		return 160934.4;
	if (LUnits == "yards" || LUnits == "yard"|| LUnits == "yd")
		return 91.44;
	if (LUnits == "feet" || LUnits == "foot" || LUnits == "ft")
		return 30.48;
	if (LUnits == "inches" || LUnits == "inch" || LUnits == "in")
		return 2.54;
	
	return 1;
}


ASpeckleUnrealActor* ASpeckleUnrealManager::CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent)
{
	const FString ObjId = Obj->GetStringField("id");
	UE_LOG(LogTemp, Log, TEXT("Creating mesh for object %s"), *ObjId);

	const FString Units = Obj->GetStringField("units");
	const float ScaleFactor = ParseScaleFactor(Units);

	const FString SpeckleType = Obj->GetStringField("speckle_type");


	
	ASpeckleUnrealActor* MeshInstance = World->SpawnActor<ASpeckleUnrealActor>(MeshActor);
	
	MeshInstance->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjId));



	//Parse Vertices
	TArray<FVector> ParsedVertices;
	int32 NumberOfVertices;
	{
		TArray<TSharedPtr<FJsonValue>> ObjectVertices = CombineChunks(Obj->GetArrayField("vertices"));
		NumberOfVertices = ObjectVertices.Num() / 3;
	
		ParsedVertices.SetNum(NumberOfVertices);

		for (size_t i = 0, j = 0; i < NumberOfVertices; i++, j += 3)
		{
			ParsedVertices[i] = FVector
			(
				ObjectVertices[j].Get()->AsNumber(),
				ObjectVertices[j + 1].Get()->AsNumber(),
				ObjectVertices[j + 2].Get()->AsNumber()
			) * ScaleFactor;
		
		}
	} 



	//Parse Triangles
	TArray<int32> ParsedTriangles;
	{
		TArray<TSharedPtr<FJsonValue>> ObjectFaces = CombineChunks(Obj->GetArrayField("faces"));
		//convert mesh faces into triangle array regardless of whether or not they are quads

		int32 j = 0;
		while (j < ObjectFaces.Num())
		{
			if (ObjectFaces[j].Get()->AsNumber() == 0)
			{
				//Triangles
				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
				j += 4;
			}
			else
			{
				//Quads to triangles
				ParsedTriangles.Add(ObjectFaces[j + 4].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());

				ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());
				ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());

				j += 5;
			}
		}
	}

	//Parse Texture Coordinates
	TArray<FVector2D> ParsedTextureCoords;
	{
		const TArray<TSharedPtr<FJsonValue>>* TextCoordArray;
		if(Obj->TryGetArrayField("textureCoordinates", TextCoordArray))
		{
			TArray<TSharedPtr<FJsonValue>> TexCoords = CombineChunks(*TextCoordArray);
			
			ParsedTextureCoords.SetNum(NumberOfVertices);
			
			const auto NumberOfTexCoords = TexCoords.Num() / 2;
			for (size_t i = 0, j = 0; i < NumberOfTexCoords; i++, j += 2)
			{
				ParsedTextureCoords[i] = FVector2D
				(
					TexCoords[j].Get()->AsNumber(),
					TexCoords[j + 1].Get()->AsNumber()
				);
			}
			
			//TODO create UV for missing TexCoords
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

	MeshInstance->SetMesh(ParsedVertices, ParsedTriangles, ParsedTextureCoords, Material);

	//UE_LOG(LogTemp, Warning, TEXT("Added %d vertices and %d triangles"), ParsedVertices.Num(), ParsedTriangles.Num());

	return MeshInstance;
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

	ASpeckleUnrealActor* BlockInstance = World->SpawnActor<ASpeckleUnrealActor>(); //TODO for now, we shall reuse ASpeckleUnrealActor because it has a root component defined in its constructor that we can use to attach the actor's parent
	BlockInstance->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjectId));

	
	BlockInstance->SetActorTransform(FTransform(TransformMatrix));
	
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



