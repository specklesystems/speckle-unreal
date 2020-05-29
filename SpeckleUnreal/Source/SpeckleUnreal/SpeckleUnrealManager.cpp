#include "SpeckleUnrealManager.h"

// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	ScaleFactor = 0.1;
}

// Called when the game starts or when spawned
void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	GetStream();
}

void ASpeckleUnrealManager::SetUpGetRequest(TSharedRef<IHttpRequest> Request)
{
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->SetHeader("Authorization", AuthToken);
}

/*Http call*/
void ASpeckleUnrealManager::GetStream()
{
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, "Downloading: " + StreamID);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	SetUpGetRequest(Request);

	Request->OnProcessRequestComplete().BindUObject(this, &ASpeckleUnrealManager::OnStreamResponseReceived);

	//This is the url on which to process the request
	Request->SetURL(ServerUrl + "streams/" + StreamID);

	Request->ProcessRequest();
}

/*Assigned function on successfull http call*/
void ASpeckleUnrealManager::OnStreamResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Stream Request failed");
		return;
	}

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> ResponseJsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, ResponseJsonObject))
	{
		//Get the value of the json object by field name
		FString ResponseMessage = ResponseJsonObject->GetStringField("message");
		TSharedPtr<FJsonObject> Stream = ResponseJsonObject->GetObjectField("resource");
		FString StreamName = Stream->GetStringField("name");
		FString StreamDescription = Stream->GetStringField("description");

		FString Units = ResponseJsonObject->GetObjectField("baseProperties")->GetStringField("units").ToLower();

		// unreal engine units are in cm by default but the conversion is editable by users so
		// this needs to be accounted for later.
		if (Units == "meters" || Units == "metres")
			ScaleFactor = 100;

		if (Units == "centimeters" || Units == "centimetres")
			ScaleFactor = 1;

		if (Units == "millimeters" || Units == "millimetres")
			ScaleFactor = 0.1;

		if (Units == "yards")
			ScaleFactor = 91.4402757;

		if (Units == "feet")
			ScaleFactor = 30.4799990;

		if (Units == "inches")
			ScaleFactor = 2.5399986;

		//Output it to the engine
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, "Units: " + FString::SanitizeFloat (ScaleFactor));
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Status: " + ResponseMessage);
		GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Green, "Name: " + StreamName);
		GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Green, "Description: " + StreamDescription);

		TArray<TSharedPtr<FJsonValue>> ObjectPlaceholderArray = Stream->GetArrayField("objects");

		GetStreamObjects(ObjectPlaceholderArray.Num());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Couldn't deserialize Json from stream response");
		GEngine->AddOnScreenDebugMessage(2, 10.0f, FColor::Red, Response->GetContentAsString());
	}
}

void ASpeckleUnrealManager::GetStreamObjects(int32 objectCount)
{
	int32 RequestLimit = 20;

	for (size_t i = 0; i < objectCount; i += RequestLimit)
	{
		TSharedRef<IHttpRequest> Request = Http->CreateRequest();

		SetUpGetRequest(Request);

		Request->OnProcessRequestComplete().BindUObject(this, &ASpeckleUnrealManager::OnStreamObjectResponseReceived);

		//This is the url on which to process the request
		Request->SetURL(ServerUrl + "streams/" + StreamID + "/objects?limit=" + FString::FromInt(RequestLimit) + "&offset=" + FString::FromInt(i));

		Request->ProcessRequest();
	}
}

void ASpeckleUnrealManager::OnStreamObjectResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Object Request failed");
		return;
	}

	//Create a pointer to hold the json serialized data
	TSharedPtr<FJsonObject> ResponseJsonObject;

	//Create a reader pointer to read the json data
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	//Deserialize the json data given Reader and the actual object to deserialize
	if (FJsonSerializer::Deserialize(Reader, ResponseJsonObject))
	{
		//Get the value of the json object by field name
		TArray<TSharedPtr<FJsonValue>> StreamObjects = ResponseJsonObject->GetArrayField("resources");

		for (size_t i = 0; i < StreamObjects.Num(); i++)
		{
			TSharedPtr<FJsonObject> StreamObject = StreamObjects[i].Get()->AsObject();

			TSharedPtr<FJsonObject> ObjectToConvert = StreamObject;

			FString objectType = ObjectToConvert->GetStringField("type");

			if (objectType.ToLower().Contains("brep"))
			{
				ObjectToConvert = StreamObject->GetObjectField("displayValue");

				objectType = ObjectToConvert->GetStringField("type");
			}

			if (objectType.ToLower().Contains("mesh"))
			{
				AActor* ActorInstance = World->SpawnActor(MeshActor);
				ASpeckleUnrealMesh* MeshInstance = (ASpeckleUnrealMesh*)ActorInstance;

				TArray<TSharedPtr<FJsonValue>> ObjectVertices = ObjectToConvert->GetArrayField("vertices");
				TArray<TSharedPtr<FJsonValue>> ObjectFaces = ObjectToConvert->GetArrayField("faces");

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
						ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
						ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());
						j += 4;
					}
					else
					{
						//Quads to triangles
						ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
						ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
						ParsedTriangles.Add(ObjectFaces[j + 2].Get()->AsNumber());

						ParsedTriangles.Add(ObjectFaces[j + 3].Get()->AsNumber());
						ParsedTriangles.Add(ObjectFaces[j + 1].Get()->AsNumber());
						ParsedTriangles.Add(ObjectFaces[j + 4].Get()->AsNumber());

						j += 5;
					}
				}

				MeshInstance->SetMesh(ParsedVerticies, ParsedTriangles);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, "Couldn't deserialize Json from object response");
		GEngine->AddOnScreenDebugMessage(2, 10.0f, FColor::Red, Response->GetContentAsString());
	}
}