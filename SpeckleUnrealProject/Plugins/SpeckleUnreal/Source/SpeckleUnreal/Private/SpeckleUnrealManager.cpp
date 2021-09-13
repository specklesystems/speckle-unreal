#include "SpeckleUnrealManager.h"

#include "MaterialConverter.h"
#include "RenderMaterial.h"

// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleMaterial(TEXT("Material'/SpeckleUnreal/SpeckleMaterial.SpeckleMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleGlassMaterial(TEXT("Material'/SpeckleUnreal/SpeckleGlassMaterial.SpeckleGlassMaterial'"));

	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	// default conversion is millimeters to centimeters because streams tend to be in ml and unreal is in cm by defaults
	ScaleFactor = 0.1;
	World = GetWorld();

	DefaultMeshMaterial = SpeckleMaterial.Object;
	BaseMeshOpaqueMaterial = SpeckleMaterial.Object;
	BaseMeshTransparentMaterial = SpeckleGlassMaterial.Object;
}

// Called when the game starts or when spawned
void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();
	
	World = GetWorld();
	ConvertedMaterials.Empty();
	
	if (ImportAtRuntime)
		ImportSpeckleObject();
	
}

/*Import the Speckle object*/
void ASpeckleUnrealManager::ImportSpeckleObject()
{
	FString url = ServerUrl + "/objects/" + StreamID + "/" + ObjectID;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, "[Speckle] Downloading: " + url);

	FHttpRequestRef Request = Http->CreateRequest();
	
	Request->SetVerb("GET");
	Request->SetHeader("Accept", TEXT("text/plain"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);

	Request->OnProcessRequestComplete().BindUObject(this, &ASpeckleUnrealManager::OnStreamTextResponseReceived);
	Request->SetURL(url);
	Request->ProcessRequest();
}

void ASpeckleUnrealManager::OnStreamTextResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Stream Request failed: " + Response->GetContentAsString());
		return;
	}
	auto responseCode = Response->GetResponseCode();
	if (responseCode != 200)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("Error response. Response code %d"), responseCode));
		return;
	}

	FString response = Response->GetContentAsString();
	
	// ParseIntoArray is very inneficient for large strings.
	// https://docs.unrealengine.com/en-US/API/Runtime/Core/Containers/FString/ParseIntoArrayLines/index.html
	// https://answers.unrealengine.com/questions/81697/reading-text-file-line-by-line.html
	// Can be fixed by setting the size of the array
	int lineCount = 0;
	for (const TCHAR* ptr = *response; *ptr; ptr++)
		if (*ptr == '\n')
			lineCount++;
	TArray<FString> lines;
	lines.Reserve(lineCount);
	response.ParseIntoArray(lines, TEXT("\n"), true);

	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("[Speckle] Parsing %d downloaded objects..."), lineCount));

	for (auto& line : lines)
	{
		FString objectId, objectJson;
		if (!line.Split("\t", &objectId, &objectJson))
			continue;
		TSharedPtr<FJsonObject> jsonObject;
		TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(objectJson);
		if (!FJsonSerializer::Deserialize(jsonReader, jsonObject))
			continue;

		SpeckleObjects.Add(objectId, jsonObject);
	}

	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("[Speckle] Converting %d objects..."), lineCount));

	ImportObjectFromCache(SpeckleObjects[ObjectID]);
	
	for (auto& m : CreatedSpeckleMeshes)
	{
		if (InProgressSpeckleMeshes.Contains(m.Key) && InProgressSpeckleMeshes[m.Key] == m.Value)
			continue;
		if (m.Value->Scene) // actors removed by the user in the editor have the Scene set to nullptr
			m.Value->Destroy();
	}

	CreatedSpeckleMeshes = InProgressSpeckleMeshes;
	InProgressSpeckleMeshes.Empty();
	
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("[Speckle] Objects imported successfully. Created %d Actors"), CreatedSpeckleMeshes.Num()));

}

ASpeckleUnrealMesh* ASpeckleUnrealManager::GetExistingMesh(const FString &objectId)
{
	if (InProgressSpeckleMeshes.Contains(objectId))
		return InProgressSpeckleMeshes[objectId];

	if(!CreatedSpeckleMeshes.Contains(objectId))
		return nullptr;
	ASpeckleUnrealMesh* meshActor = CreatedSpeckleMeshes[objectId];
	// Check if actor has been deleted by the user
	if (!meshActor || !meshActor->Scene)
		return nullptr;
	return meshActor;
}


void ASpeckleUnrealManager::ImportObjectFromCache(const TSharedPtr<FJsonObject> speckleObj, const URenderMaterial* FallbackMaterial)
{
	if (!speckleObj->HasField("speckle_type"))
		return;
	if (speckleObj->GetStringField("speckle_type") == "reference" && speckleObj->HasField("referencedId")) {
		TSharedPtr<FJsonObject> referencedObj;
		if (SpeckleObjects.Contains(speckleObj->GetStringField("referencedId")))
			ImportObjectFromCache(SpeckleObjects[speckleObj->GetStringField("referencedId")], FallbackMaterial);		
		return;
	}
	if (!speckleObj->HasField("id"))
		return;
	FString objectId = speckleObj->GetStringField("id");
	FString speckleType = speckleObj->GetStringField("speckle_type");
	
	// UE_LOG(LogTemp, Warning, TEXT("Importing object %s (type %s)"), *objectId, *speckleType);
	
	//Set a fallback material, in-case children doesn't have a material
	if (speckleObj->HasField("renderMaterial"))
		FallbackMaterial = UMaterialConverter::ParseRenderMaterial(speckleObj->GetObjectField("renderMaterial"));

	
	if (speckleObj->GetStringField("speckle_type") == "Objects.Geometry.Mesh") {
		ASpeckleUnrealMesh* mesh = GetExistingMesh(objectId);
		if (!mesh)
			mesh = CreateMesh(speckleObj, FallbackMaterial);
		InProgressSpeckleMeshes.Add(objectId, mesh);
		return;
	}

	if (speckleObj->HasField("@displayMesh"))
	{

		// Check if the @displayMesh is an object or an array
		const TSharedPtr<FJsonObject> *meshObjPtr;
		const TArray<TSharedPtr<FJsonValue>> *meshArrayPtr;

		if (speckleObj->TryGetObjectField("@displayMesh", meshObjPtr))
		{
			TSharedPtr<FJsonObject> meshObj = SpeckleObjects[(*meshObjPtr)->GetStringField("referencedId")];

			ASpeckleUnrealMesh* mesh = GetExistingMesh(objectId);
			if (!mesh)
				mesh = CreateMesh(meshObj, FallbackMaterial);
			InProgressSpeckleMeshes.Add(objectId, mesh);
		}
		else if (speckleObj->TryGetArrayField("@displayMesh", meshArrayPtr))
		{
			for (auto& meshObjValue : *meshArrayPtr)
			{
				FString meshId = meshObjValue->AsObject()->GetStringField("referencedId");
				FString unrealMeshKey = objectId + meshId;

				ASpeckleUnrealMesh* mesh = GetExistingMesh(unrealMeshKey);
				if (!mesh)
					mesh = CreateMesh(SpeckleObjects[meshId], FallbackMaterial);
				InProgressSpeckleMeshes.Add(unrealMeshKey, mesh);
			}
		}
	}

	// Go recursively into all object fields (except @displayMesh)
	for (auto& kv : speckleObj->Values)
	{
		if (kv.Key == "@displayMesh")
			continue;

		const TSharedPtr< FJsonObject > *subObjectPtr;
		if (kv.Value->TryGetObject(subObjectPtr))
		{
			ImportObjectFromCache(*subObjectPtr, FallbackMaterial);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>> *subArrayPtr;
		if (kv.Value->TryGetArray(subArrayPtr))
		{
			for (auto& arrayElement : *subArrayPtr)
			{
				const TSharedPtr<FJsonObject> *arraySubObjPtr;
				if (!arrayElement->TryGetObject(arraySubObjPtr))
					continue;
				ImportObjectFromCache(*arraySubObjPtr, FallbackMaterial);
			}
		}
	}

}

UMaterialInterface* ASpeckleUnrealManager::CreateMaterial(TSharedPtr<FJsonObject> RenderMaterialObject, bool AcceptMaterialOverride)
{
	if (RenderMaterialObject->GetStringField("speckle_type") == "reference")
		RenderMaterialObject = SpeckleObjects[RenderMaterialObject->GetStringField("referencedId")];

	//Parse to a URenderMaterial
	const URenderMaterial* SpeckleMaterial = UMaterialConverter::ParseRenderMaterial(RenderMaterialObject);

	return CreateMaterial(SpeckleMaterial, AcceptMaterialOverride);
}

UMaterialInterface* ASpeckleUnrealManager::CreateMaterial(const URenderMaterial* SpeckleMaterial, bool AcceptMaterialOverride)
{
	const auto materialID = SpeckleMaterial->ObjectID;

	
	if(AcceptMaterialOverride)
	{
		//Override by id
		if(MaterialOverridesById.Contains(materialID))
		{
			return MaterialOverridesById[materialID];
		}
		//Override by name
		const FString Name = SpeckleMaterial->Name;
		for (UMaterialInterface* Mat : MaterialOverridesByName)
		{
			if(Mat->GetName() == Name) return Mat;
		}
	}


	if(ConvertedMaterials.Contains(materialID))
	{
		return ConvertedMaterials[materialID];
	}
	
	//Create Convert Material Instance
	UMaterialInterface* ExplicitMaterial;
	if(SpeckleMaterial->Opacity >= 1)
		ExplicitMaterial = BaseMeshOpaqueMaterial;
	else
		ExplicitMaterial = BaseMeshTransparentMaterial;
		
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(ExplicitMaterial, this, FName(SpeckleMaterial->Name));
	UMaterialConverter::AssignPropertiesFromSpeckle(DynMaterial, SpeckleMaterial);

	ConvertedMaterials.Add(materialID, DynMaterial);
	
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


ASpeckleUnrealMesh* ASpeckleUnrealManager::CreateMesh(const TSharedPtr<FJsonObject> Obj, const URenderMaterial* FallbackMaterial)
{
	const FString ObjId = Obj->GetStringField("id");
	UE_LOG(LogTemp, Log, TEXT("Creating mesh for object %s"), *ObjId);

	FString Units = Obj->GetStringField("units");
	// unreal engine units are in cm by default but the conversion is editable by users so
	// this needs to be accounted for later.
		
	ScaleFactor = 1;
	if (Units == "meters" || Units == "metres" || Units == "m")
		ScaleFactor = 100;

	if (Units == "centimeters" || Units == "centimetres" || Units == "cm")
		ScaleFactor = 1;

	if (Units == "millimeters" || Units == "millimetres" || Units == "mm")
		ScaleFactor = 0.1;

	if (Units == "yards" || Units == "yd")
		ScaleFactor = 91.4402757;

	if (Units == "feet" || Units == "ft")
		ScaleFactor = 30.4799990;

	if (Units == "inches" || Units == "in")
		ScaleFactor = 2.5399986;

	// The following line can be used to debug large objects
	// ScaleFactor = ScaleFactor * 0.1;
	

	AActor* ActorInstance = World->SpawnActor(MeshActor);
	ASpeckleUnrealMesh* MeshInstance = static_cast<ASpeckleUnrealMesh*>(ActorInstance);
	
	//MeshInstance->Rename(*ObjId, Parent, REN_None);
	//if(Parent) MeshInstance->AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);
	
	//Currently not needed since meshes are placed under this actor.
	// #if WITH_EDITOR
	// 	MeshInstance->SetFolderPath(FName(GetActorLabel() + FString(TEXT("_")) + StreamID));
	// #endif

	
	// attaches each speckleMesh under this actor (SpeckleManager)
	if(MeshInstance != nullptr)
	{
		MeshInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		MeshInstance->SetOwner(this);
	}


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


	// Material priority (low to high): DefaultMeshMaterial, Material set on parent, Converted RenderMaterial set on mesh, MaterialOverride match
	UMaterialInterface* Material;
	
	if (Obj->HasField("renderMaterial"))
	{
		const auto RenderMatObj = Obj->GetObjectField("renderMaterial");
		Material = CreateMaterial(RenderMatObj);
	}
	else
	{
		if (FallbackMaterial)
			Material = CreateMaterial(FallbackMaterial);
		else
			Material = DefaultMeshMaterial;
	}
	
	MeshInstance->SetMesh(ParsedVertices, ParsedTriangles, Material, FLinearColor::White);

	//UE_LOG(LogTemp, Warning, TEXT("Added %d vertices and %d triangles"), ParsedVertices.Num(), ParsedTriangles.Num());

	return MeshInstance;
}

void ASpeckleUnrealManager::DeleteObjects()
{

	ConvertedMaterials.Empty();
	for (auto& m : CreatedSpeckleMeshes)
	{
		if (m.Value->Scene)
			m.Value->Destroy();
	}

	CreatedSpeckleMeshes.Empty();
	InProgressSpeckleMeshes.Empty();
}
