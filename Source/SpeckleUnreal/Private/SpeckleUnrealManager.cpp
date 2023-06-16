
#include "SpeckleUnrealManager.h"

#include "ReceiveSelectionComponent.h"
#include "API/Operations/ReceiveOperation.h"
#include "Transports/MemoryTransport.h"
#include "Transports/ServerTransport.h"
#include "LogSpeckle.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/SpeckleConverterComponent.h"
#include "Misc/ScopedSlowTask.h"
#include "Objects/Base.h"
#include "Mixpanel.h"
#include "API/SpeckleAPIFunctions.h"
#include "Engine/Engine.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"


// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	RootComponent->SetRelativeScale3D(FVector(1,1,1));
    RootComponent->SetMobility(EComponentMobility::Static);

	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));
	ReceiveSelection = CreateDefaultSubobject<UReceiveSelectionComponent>(FName("ReceiveSelection"));
	KeepCache = true;
	DisplayProgressBar = true;
}

void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	if(ImportAtRuntime) Receive();
}

void ASpeckleUnrealManager::Receive()
{
	// Check object to receive has been specified properly
	FString StatusMessage;
	if(!ReceiveSelection->IsSelectionComplete(StatusMessage))
	{
		HandleError(StatusMessage);
		return;
	}

	// Delete all actors from previous receive operations
	DeleteObjects();
	
	const FString ServerUrl = ReceiveSelection->ServerUrl;
	const FString AuthToken = ReceiveSelection->AuthToken;
	const FString StreamId = ReceiveSelection->StreamId;
	const FString ObjectId = ReceiveSelection->ObjectId;
	
	FAnalytics::TrackEvent( ServerUrl, "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }, {"worldType", FString::FromInt(GetWorld()->WorldType)}});
	FString Message = FString::Printf(TEXT("Fetching Objects from Speckle Server: %s"), *ServerUrl);
	PrintMessage(Message);

	// Setup network transports
	UServerTransport* ServerTransport = UServerTransport::CreateServerTransport(ServerUrl, StreamId, AuthToken);
	
	if(!KeepCache && LocalObjectCache)
	{
		LocalObjectCache.GetObjectRef()->ConditionalBeginDestroy();
		LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();
	}
	if(!LocalObjectCache) LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();

	// Setup delegate callbacks
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleError);
	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleReceive, DisplayProgressBar);
	
	// Send request for objects
	ServerTransport->CopyObjectAndChildren(ObjectId, LocalObjectCache, CompleteDelegate, ErrorDelegate);

	// Read receipt (if receiving a commit object)
	FSpeckleCommit Commit;
	if(ReceiveSelection->TryGetSelectedCommit(Commit))
	{
		//TODO read receipt
		USpeckleAPIFunctions::CommitReceived(Commit);
	}
}



void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> RootObject, bool DisplayProgress)
{
	if(RootObject == nullptr) return;
	
	const UBase* Res = USpeckleSerializer::DeserializeBase(RootObject, LocalObjectCache);
	if(IsValid(Res))
	{
		Converter->RecursivelyConvertToNative(this, Res, LocalObjectCache, DisplayProgress, Actors);
		
		FString Message = FString::Printf(TEXT("Converted %d Actors"), Actors.Num());
		PrintMessage(Message);
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

void ASpeckleUnrealManager::PrintMessage(FString& Message, bool IsError) const
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
	Converter->FinishConversion();
	
	for (AActor* a : Actors)
	{
		if(IsValid(a)) a->Destroy();
	}

	Actors.Empty();
}

#undef LOCTEXT_NAMESPACE