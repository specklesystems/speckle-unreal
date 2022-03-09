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
	RootComponent->SetRelativeScale3D(FVector(1,-1,1)); // Flip the Y because Speckle uses right handed coordinates, unreal uses left handed
    RootComponent->SetMobility(EComponentMobility::Static);

	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));

	ServerUrl = "https://speckle.xyz";
}

void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	if(ImportAtRuntime) Receive();
}

void ASpeckleUnrealManager::Receive()
{
	DeleteObjects();

	// Trim parameters
	ServerUrl.TrimEndInline();
	while(ServerUrl.RemoveFromEnd("/")) { }
	StreamID.TrimEndInline();
	ObjectID.TrimEndInline();
	AuthToken.TrimEndInline();
	
	// Try and get object from local cache
	if(LocalObjectCache.GetInterface() == nullptr) LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();
	
	auto Obj = LocalObjectCache->GetSpeckleObject(ObjectID);
	if (Obj != nullptr )
	{
		HandleReceive(Obj);
		return;
	}
	
	
	// Try and get object from server
	TScriptInterface<ITransport> ServerTransport = UServerTransport::CreateServerTransport(ServerUrl,StreamID,AuthToken);

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleReceive);
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleError);
	
	ServerTransport->CopyObjectAndChildren(ObjectID, LocalObjectCache, CompleteDelegate, ErrorDelegate);
	
}


void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> RootObject)
{
	if(RootObject == nullptr) return;
	
	const UBase* Res = USpeckleSerializer::DeserializeBase(RootObject, LocalObjectCache);
	if(IsValid(Res))
	{
		Converter->RecursivelyConvertToNative(this, Res, LocalObjectCache, Actors);
		FString Message = FString::Printf(TEXT("Converted %d Actors"), Actors.Num());
		PrintMessage(Message, false);
	}
	else
	{
		FString Id;
		RootObject->TryGetStringField("id", Id);
		FString Message = FString::Printf(TEXT("Failed to deserialise root object: %s"), *Id);
		HandleError(Message);
	}
}

void ASpeckleUnrealManager::HandleError(FString& Message)
{
	PrintMessage(Message, true);
}

void ASpeckleUnrealManager::PrintMessage(FString& Message, bool IsError)
{
	if(IsError)
	{
		UE_LOG(LogSpeckle, Error, TEXT("%s"), *Message);
	}
	else
	{
		UE_LOG(LogSpeckle, Log, TEXT("%s"), *Message);
	}

	FColor Color = IsError? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, Color, Message);
}


void ASpeckleUnrealManager::DeleteObjects()
{
	Converter->CleanUp();
	
	for (AActor* a : Actors)
	{
		a->Destroy();
	}

	Actors.Empty();
}
