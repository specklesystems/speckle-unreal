#include "SpeckleUnrealManager.h"

#include "API/Operations/ReceiveOperation.h"
#include "Transports/MemoryTransport.h"
#include "Transports/ServerTransport.h"
#include "LogSpeckle.h"
#include "Alembic/Abc/Base.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/SpeckleConverterComponent.h"
#include "Objects/Base.h"


// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
	RootComponent->SetRelativeScale3D(FVector(-1,1,1)); // Flip the X because Speckle uses right handed coordinates, unreal uses left handed
    RootComponent->SetMobility(EComponentMobility::Static);

	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));

	ServerUrl = "https://speckle.xyz";
}

void ASpeckleUnrealManager::Receive()
{
	DeleteObjects();

	// Setup Transports
	if(LocalObjectCache.GetInterface() != nullptr)
	{
		LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();
	}
	
	TScriptInterface<ITransport> ServerTransport = UServerTransport::CreateServerTransport(ServerUrl,StreamID,AuthToken);

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleReceive);
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleError);
	
	ServerTransport->CopyObjectAndChildren(ObjectID, LocalObjectCache, CompleteDelegate, ErrorDelegate);
	
}


void ASpeckleUnrealManager::HandleError(FString& Message)
{
	UE_LOG(LogSpeckle, Error, TEXT("%s"), *Message);
}

void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> RootObject)
{
	if(RootObject == nullptr) return;
	UBase* Res = USpeckleSerializer::DeserializeBase(RootObject, LocalObjectCache);
	if(IsValid(Res))
	{
		Converter->RecursivelyConvertToNative(this, Res, LocalObjectCache, Actors);
	}
	else
	{
		FString Id;
		RootObject->TryGetStringField("id", Id);
		UE_LOG(LogSpeckle, Error, TEXT("Failed to deserialise root object: %s"), *Id);
	}
}

void ASpeckleUnrealManager::DeleteObjects()
{
	Converter->DeleteObjects();
	
	for (AActor* a : Actors)
	{
		a->Destroy();
	}

	Actors.Empty();
}

void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	if(ImportAtRuntime) Receive();
}
