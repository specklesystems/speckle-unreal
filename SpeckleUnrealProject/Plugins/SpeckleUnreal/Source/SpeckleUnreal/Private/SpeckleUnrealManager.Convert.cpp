#include "SpeckleUnrealManager.h"

#include "MaterialConverter.h"
#include "SpeckleUnrealMesh.h"


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
	
	AActor* Native = nullptr;


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

	
	if(IsValid(Native))
	{
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

		return 1;
	};

	return ParseUnits(Units.ToLower()) * WorldToCentimeters;
}


ASpeckleUnrealMesh* ASpeckleUnrealManager::CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent)
{
	const FString ObjId = Obj->GetStringField("id");
	UE_LOG(LogTemp, Log, TEXT("Creating mesh for object %s"), *ObjId);

	const FString Units = Obj->GetStringField("units");
	const float ScaleFactor = ParseScaleFactor(Units);

	const FString SpeckleType = Obj->GetStringField("speckle_type");


	
	ASpeckleUnrealMesh* MeshInstance = World->SpawnActor<ASpeckleUnrealMesh>(MeshActor);
	
	MeshInstance->SetActorLabel(FString::Printf(TEXT("%s - %s"), *SpeckleType, *ObjId));



	//Parse Vertices
	TArray<FVector> ParsedVertices;
	int32 NumberOfVertices;
	{
		TArray<TSharedPtr<FJsonValue>> ObjectVertices = CombineChunks(Obj->GetArrayField("vertices"));
		NumberOfVertices = ObjectVertices.Num() / 3;
	
		ParsedVertices.Reserve(NumberOfVertices);

		for (size_t i = 0, j = 0; i < NumberOfVertices; i++, j += 3)
		{
			ParsedVertices.Add(FVector
			(
				ObjectVertices[j].Get()->AsNumber(),
				ObjectVertices[j + 1].Get()->AsNumber(),
				ObjectVertices[j + 2].Get()->AsNumber()
			) * ScaleFactor);
		
		}
	} 
	
	//Parse Texture Coordinates
	TArray<FVector2D> ParsedTextureCoords;
	{
		const TArray<TSharedPtr<FJsonValue>>* TextCoordArray;
		if(Obj->TryGetArrayField("textureCoordinates", TextCoordArray))
		{
			TArray<TSharedPtr<FJsonValue>> TexCoords = CombineChunks(*TextCoordArray);
			
			ParsedTextureCoords.Reserve(TexCoords.Num() / 2);
			
			for (size_t i = 0; i < TexCoords.Num() - 1; i += 2)
			{
				ParsedTextureCoords.Add(FVector2D
				(
					TexCoords[i].Get()->AsNumber(),
					TexCoords[i + 1].Get()->AsNumber()
				));
			}

		}
	}

	//Array of Faces (Tuple of Vertex index, TexCoord index)
	TArray<TArray<TTuple<int32,int32>>> ParsedPolygons;
	
	{
		TArray<TSharedPtr<FJsonValue>> FaceVertices = CombineChunks(Obj->GetArrayField("faces"));
		ParsedPolygons.Reserve(FaceVertices.Num() / 3); //Reserve space assuming faces will all be triangles
		
		int32 NIndex = 0, TIndex = 0;
		while (NIndex < FaceVertices.Num()) //TODO some sort of check that there are N more vertices
		{			
			//Number of vertices in polygon
			int n = FaceVertices[NIndex].Get()->AsNumber();
			if(n < 3) n += 3; // 0 -> 3, 1 -> 4

			TArray<TTuple<int32,int32>> Polygon;
			Polygon.Reserve(n);
			
			for(int i = 0; i < n; i++)
			{
				Polygon.Add(TTuple<int32,int32>(
					FaceVertices[NIndex + i + 1].Get()->AsNumber(),
					TIndex + i)
					);
			}
			NIndex += n + 1;
			TIndex += n;
			ParsedPolygons.Add(Polygon);
		}

		//Fill missing tex coords with default values
		ParsedTextureCoords.SetNum(TIndex);
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

	MeshInstance->SetMesh(ParsedVertices, ParsedPolygons, ParsedTextureCoords, Material);

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

	ASpeckleUnrealActor* BlockInstance = World->SpawnActor<ASpeckleUnrealActor>();
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



