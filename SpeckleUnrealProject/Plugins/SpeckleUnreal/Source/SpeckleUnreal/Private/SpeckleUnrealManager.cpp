#include "SpeckleUnrealManager.h"

#include "MaterialConverter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleMaterial(TEXT("Material'/SpeckleUnreal/SpeckleMaterial.SpeckleMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleGlassMaterial(TEXT("Material'/SpeckleUnreal/SpeckleGlassMaterial.SpeckleGlassMaterial'"));

	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
	RootComponent->SetRelativeScale3D(FVector(-1,1,1));
    RootComponent->SetMobility(EComponentMobility::Static); 
    
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
	
	if (ImportAtRuntime)
		ImportSpeckleObject();
	
}

/*Import the Speckle object*/
void ASpeckleUnrealManager::ImportSpeckleObject()
{
	const FString UserAgent = FString::Printf(TEXT("Unreal Engine (%s) / %d.%d.%d"), *UGameplayStatics::GetPlatformName(), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION);

#if !SUPPRESS_SPECKLE_ANALYTICS
	

	const FString HostApplication = FString::Printf(TEXT("Unreal%%20Engine%%20%d"), ENGINE_MAJOR_VERSION);
	const FString Action = "receive/manual";
	FString SpeckleUserID = "No%20SUID";
	
#if PLATFORM_WINDOWS
	const FString UserPath = UKismetSystemLibrary::GetPlatformUserDir().LeftChop(10); //remove "Documents/"
	const FString Dir = FString::Printf(TEXT("%sAppData/Roaming/Speckle/suuid"), *UserPath);
	FFileHelper::LoadFileToString(SpeckleUserID, *Dir);
#endif	
	//TODO MACOS
	
	//Track page view
	const FString ViewURL = FString::Printf(
		TEXT("https://speckle.matomo.cloud/matomo.php?idsite=2&rec=1&apiv=1&uid=%s&action_name=%s&url=http://connectors/%s/%s&urlref=http://connectors/%s/%s&_cvar=%%7B%%22hostApplication%%22:%%20%%22%s%%22%%7D"),
		*SpeckleUserID,
		*Action,
		*HostApplication,
		*Action,
		*HostApplication,
		*Action,
		*HostApplication
	);

	const FHttpRequestRef ViewTrackingRequest = Http->CreateRequest();
	ViewTrackingRequest->SetVerb("POST");
	ViewTrackingRequest->SetURL(ViewURL);
	ViewTrackingRequest->SetHeader("User-Agent", UserAgent);
	ViewTrackingRequest->ProcessRequest();
	
	//Track receive action
	const FString EventURL = FString::Printf(
		TEXT("https://speckle.matomo.cloud/matomo.php?idsite=2&rec=1&apiv=1&uid=%s&_cvar=%%7B%%22hostApplication%%22:%%20%%22%s%%22%%7D&e_c=%s&e_a=%s"),
			*SpeckleUserID,
			*HostApplication,
			*HostApplication,
			*Action
		);
	
	const FHttpRequestRef EventTrackingRequest = Http->CreateRequest();
	EventTrackingRequest->SetVerb("POST");
	EventTrackingRequest->SetURL(EventURL);
	EventTrackingRequest->SetHeader("User-Agent", UserAgent);
	
	EventTrackingRequest->ProcessRequest();
	
#endif

	
	const FString url = ServerUrl + "/objects/" + StreamID + "/" + ObjectID;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, "[Speckle] Downloading: " + url);
	
	const FHttpRequestRef Request = Http->CreateRequest();
	
	Request->SetVerb("GET");
	Request->SetHeader("Accept", TEXT("text/plain"));
	Request->SetHeader("Authorization", "Bearer " + AuthToken);

	Request->OnProcessRequestComplete().BindUObject(this, &ASpeckleUnrealManager::OnStreamTextResponseReceived);
	Request->SetURL(url);
	Request->SetHeader("User-Agent", UserAgent);
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


	for (const auto& line : lines)
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

	//World Units setup
	WorldToCentimeters = 1; //Default value of 1uu = 1cm

	AWorldSettings* WorldSettings;
	if( ( IsValid(World) || IsValid(World = GetWorld()) )
		&& IsValid(WorldSettings = World->GetWorldSettings()) )
	{
		WorldToCentimeters = WorldSettings->WorldToMeters / 100;
	}
	
	
	ImportObjectFromCache(this, SpeckleObjects[ObjectID]);
	
	for (auto& m : CreatedObjectsCache)
	{
		if(AActor* a = Cast<AActor>(m))
			a->Destroy();
		else
			m->ConditionalBeginDestroy();
	}

	CreatedObjectsCache = InProgressObjectsCache;
	InProgressObjectsCache.Empty();
	
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("[Speckle] Objects imported successfully. Created %d Actors"), CreatedObjectsCache.Num()));

}



void ASpeckleUnrealManager::DeleteObjects()
{
	ConvertedMaterials.Empty();
	
	for (const auto& m : CreatedObjectsCache)
	{
		if(AActor* a = Cast<AActor>(m))
			a->Destroy();
		else
			m->ConditionalBeginDestroy();
		
	}

	CreatedObjectsCache.Empty();
}
