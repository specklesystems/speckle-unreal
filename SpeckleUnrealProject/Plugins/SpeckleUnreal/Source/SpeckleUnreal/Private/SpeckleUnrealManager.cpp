#include "SpeckleUnrealManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Dom/JsonObject.h"

// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleMaterial(TEXT("Material'/SpeckleUnreal/SpeckleMaterial.SpeckleMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleGlassMaterial(TEXT("Material'/SpeckleUnreal/SpeckleGlassMaterial.SpeckleGlassMaterial'"));

	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	// default conversion is millimeters to centimeters because streams tend to be in ml and unreal is in cm by defaults
	ScaleFactor = 0.1;
	World = AActor::GetWorld();

	DefaultMeshOpaqueMaterial = SpeckleMaterial.Object;
	DefaultMeshTransparentMaterial = SpeckleGlassMaterial.Object;
}

// Called when the game starts or when spawned
void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	if (ImportAtRuntime)
		ImportSpeckleObject();
}

/*Import the Speckle object*/
void ASpeckleUnrealManager::ImportSpeckleObject()
{
	FString url = ServerUrl + "/objects/" + StreamID + "/" + ObjectID;
	
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

void ASpeckleUnrealManager::ImportObjectFromCache(const TSharedPtr<FJsonObject> speckleObj)
{
	if (!speckleObj->HasField("speckle_type"))
		return;
	if (speckleObj->GetStringField("speckle_type") == "reference" && speckleObj->HasField("referencedId")) {
		TSharedPtr<FJsonObject> referencedObj;
		if (SpeckleObjects.Contains(speckleObj->GetStringField("referencedId")))
			ImportObjectFromCache(SpeckleObjects[speckleObj->GetStringField("referencedId")]);		
		return;
	}
	if (!speckleObj->HasField("id"))
		return;
	FString objectId = speckleObj->GetStringField("id");
	FString speckleType = speckleObj->GetStringField("speckle_type");
	
	// UE_LOG(LogTemp, Warning, TEXT("Importing object %s (type %s)"), *objectId, *speckleType);

	if (speckleObj->GetStringField("speckle_type") == "Objects.Geometry.Mesh") {
		ASpeckleUnrealMesh* mesh = GetExistingMesh(objectId);
		if (!mesh)
			mesh = CreateMesh(speckleObj);
		InProgressSpeckleMeshes.Add(objectId, mesh);
		return;
	}

	if (speckleObj->HasField("@displayMesh"))
	{
		UMaterialInterface* explicitMaterial = nullptr;
		if (speckleObj->HasField("renderMaterial"))
			explicitMaterial = CreateMaterial(speckleObj->GetObjectField("renderMaterial"));

		// Check if the @displayMesh is an object or an array
		const TSharedPtr<FJsonObject> *meshObjPtr;
		const TArray<TSharedPtr<FJsonValue>> *meshArrayPtr;

		if (speckleObj->TryGetObjectField("@displayMesh", meshObjPtr))
		{
			TSharedPtr<FJsonObject> meshObj = SpeckleObjects[(*meshObjPtr)->GetStringField("referencedId")];

			ASpeckleUnrealMesh* mesh = GetExistingMesh(objectId);
			if (!mesh)
				mesh = CreateMesh(meshObj, explicitMaterial);
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
					mesh = CreateMesh(SpeckleObjects[meshId], explicitMaterial);
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
			ImportObjectFromCache(*subObjectPtr);
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
				ImportObjectFromCache(*arraySubObjPtr);
			}
		}
	}

}

UMaterialInterface* ASpeckleUnrealManager::CreateMaterial(TSharedPtr<FJsonObject> obj)
{
	if (obj->GetStringField("speckle_type") == "reference")
		obj = SpeckleObjects[obj->GetStringField("referencedId")];

	int opacity;
	if (obj->TryGetNumberField("opacity", opacity)) {
		if (opacity < 1) {
			return DefaultMeshTransparentMaterial;
		}
	}
	return DefaultMeshOpaqueMaterial;
}

ASpeckleUnrealMesh* ASpeckleUnrealManager::CreateMesh(TSharedPtr<FJsonObject> obj, UMaterialInterface* explicitMaterial)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating mesh for object %s"), *obj->GetStringField("id"));

	FString Units = obj->GetStringField("units");
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

	FString verticesId = obj->GetArrayField("vertices")[0]->AsObject()->GetStringField("referencedId");
	FString facesId = obj->GetArrayField("faces")[0]->AsObject()->GetStringField("referencedId");

	TArray<TSharedPtr<FJsonValue>> ObjectVertices = SpeckleObjects[verticesId]->GetArrayField("data");
	TArray<TSharedPtr<FJsonValue>> ObjectFaces = SpeckleObjects[facesId]->GetArrayField("data");
	
#if WITH_EDITOR
	World = GetWorld();
#endif

	if(World == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Null World")));
		return nullptr;
	}
	
	AActor* ActorInstance = World->SpawnActor(MeshActor);
	ASpeckleUnrealMesh* MeshInstance = (ASpeckleUnrealMesh*)ActorInstance;
	
	// attaches each speckleMesh under this actor (SpeckleManager)
	if(MeshInstance != nullptr)
	{
		MeshInstance->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		MeshInstance->SetOwner(this);
	}
	
	TArray<FVector> ParsedVerticies;

	for (size_t j = 0; j < ObjectVertices.Num(); j += 3)
	{
		ParsedVerticies.Add(FVector
		(
			(float)(ObjectVertices[j].Get()->AsNumber()) * -1,
			(float)(ObjectVertices[j + 1].Get()->AsNumber()),
			(float)(ObjectVertices[j + 2].Get()->AsNumber())
		) * ScaleFactor);
	}

	//convert mesh faces into triangle array regardless of whether or not they are quads
	TArray<int32> ParsedTriangles;
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

	// Material priority (low to high): DefaultMeshOpaqueMaterial, renderMaterial set on parent, renderMaterial set on mesh
	if (!explicitMaterial)
		explicitMaterial = DefaultMeshOpaqueMaterial;
	if (obj->HasField("renderMaterial"))
		explicitMaterial = CreateMaterial(obj->GetObjectField("renderMaterial"));

	MeshInstance->SetMesh(ParsedVerticies, ParsedTriangles, explicitMaterial, FLinearColor::White);

	// UE_LOG(LogTemp, Warning, TEXT("Added %d vertices and %d triangles"), ParsedVerticies.Num(), ParsedTriangles.Num());

	return MeshInstance;
}

void ASpeckleUnrealManager::OnStreamCommitsListResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Stream Request failed: " + Response->GetContentAsString());
		return;
	}

	auto responseCode = Response-> GetResponseCode();
	if (responseCode != 200)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("Error response. Response code %d"), responseCode));
		return;
	}

	FString response = Response->GetContentAsString();
	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> JsonObject;
	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(response);

	ArrayOfCommits.Empty();
	
	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		for(const auto& pair:JsonObject->Values)
		{
			auto CommitsArr = JsonObject->GetObjectField(TEXT("data"))
			->GetObjectField(TEXT("stream"))
			->GetObjectField(TEXT("commits"))
			->GetArrayField(TEXT("items"));

			for (auto commit : CommitsArr)
			{
				auto ObjID = commit->AsObject()->GetStringField("referencedObject");
				auto Message = commit->AsObject()->GetStringField("message");
				auto AuthorName = commit->AsObject()->GetStringField("authorName");
				auto Commit = FSpeckleCommit(ObjID, AuthorName, Message);
				ArrayOfCommits.Add(Commit);
			}
		}
	}
}

void ASpeckleUnrealManager::FetchCommits()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Green, "Fetching commits...");
	}
	
	FString url = ServerUrl + "/graphql";

	FHttpRequestRef Request = Http->CreateRequest();

	Request->SetVerb(TEXT("POST"));
	Request->SetHeader("Accept-Encoding", TEXT("gzip"));
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Accept", TEXT("application/json"));
	Request->SetHeader("DNT", TEXT("1"));
	Request->SetHeader("Origin", TEXT("https://speckle.xyz"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);

	FString postPayload = "{\"query\": \"query{\\n stream (id: \\\"" + StreamID + "\\\"){\\n id\\n name\\n commits{\\n totalCount\\n cursor\\n items{\\n id\\n referencedObject\\n authorName\\n message\\n }\\n }\\n }\\n}\"}";
	Request->SetContentAsString(postPayload);

	Request->OnProcessRequestComplete().BindUObject(this, &ASpeckleUnrealManager::OnStreamCommitsListResponseReceived);
	Request->SetURL(url);
	Request->ProcessRequest();

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Green, "Fetched commits");
	}
}

void ASpeckleUnrealManager::DeleteObjects()
{
	for (auto& m : CreatedSpeckleMeshes)
	{
		if (m.Value->Scene)
			m.Value->Destroy();
	}

	CreatedSpeckleMeshes.Empty();
	InProgressSpeckleMeshes.Empty();
}
