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
	RootComponent->SetRelativeScale3D(FVector(-1,1,1));
    RootComponent->SetMobility(EComponentMobility::Static);

	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));
}

void ASpeckleUnrealManager::ReceiveCPP()
{
	TScriptInterface<ITransport> ServerTransport = UServerTransport::CreateServerTransport(ServerUrl,StreamID,AuthToken);

	MemoryTransport = UMemoryTransport::CreateEmptyMemoryTransport();

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

void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	UBase* Res = FSpeckleSerializer::DeserializeBase(Object, MemoryTransport);
	if(IsValid(Res))
	{
		TArray<AActor*> Actors;
		Converter->RecursivelyConvertToNative(this, Res, MemoryTransport, Actors);
	}
	else
	{
		UE_LOG(LogSpeckle, Error, TEXT("Failed to deserialise root object"));
	}
}




