#include "SpeckleUnrealManager.h"

#include "API/Operations/ReceiveOperation.h"
#include "Transports/MemoryTransport.h"
#include "Transports/ServerTransport.h"
#include "LogSpeckle.h"
#include "Alembic/Abc/Base.h"
#include "API/SpeckleSerializer.h"
#include "Objects/Base.h"


// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
	RootComponent->SetRelativeScale3D(FVector(-1,1,1)); // Flip the X because Speckle uses right handed coordinates, unreal uses left handed
    RootComponent->SetMobility(EComponentMobility::Static);

	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));
}

void ASpeckleUnrealManager::ReceiveCPP()
{
	DeleteObjects();

	// Setup Transports
	MemoryTransport = UMemoryTransport::CreateEmptyMemoryTransport();
	TScriptInterface<ITransport> ServerTransport = UServerTransport::CreateServerTransport(ServerUrl,StreamID,AuthToken);

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleReceive);
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleError);
	
	ServerTransport->CopyObjectAndChildren(ObjectID, MemoryTransport, CompleteDelegate, ErrorDelegate);
	
}


void ASpeckleUnrealManager::HandleError(FString& Message)
{
	UE_LOG(LogSpeckle, Error, TEXT("%s"), *Message);
}

void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> RootObject)
{
	UBase* Res = FSpeckleSerializer::DeserializeBase(RootObject, MemoryTransport);
	if(IsValid(Res))
	{
		TArray<AActor*> Actors;
		Converter->RecursivelyConvertToNative(this, Res, MemoryTransport, Actors);
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
	
	for (const auto m : CreatedObjectsCache)
	{
		if(AActor* a = Cast<AActor>(m))
			a->Destroy();
		else
			m->ConditionalBeginDestroy();
		
	}

	CreatedObjectsCache.Empty();
}
