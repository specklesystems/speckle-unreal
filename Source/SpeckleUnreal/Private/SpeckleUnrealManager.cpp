#include "SpeckleUnrealManager.h"

#include "API/Operations/ReceiveOperation.h"
#include "Transports/MemoryTransport.h"
#include "Transports/ServerTransport.h"
#include "LogSpeckle.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/SpeckleConverterComponent.h"
#include "Misc/ScopedSlowTask.h"
#include "Objects/Base.h"
#include "Mixpanel.h"
#include "Engine/Engine.h"
#include "Interfaces/IHttpRequest.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"


// Sets default values
ASpeckleUnrealManager::ASpeckleUnrealManager()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	RootComponent->SetRelativeScale3D(FVector(1,1,1));
    RootComponent->SetMobility(EComponentMobility::Static);

	// Convert JSON to object models
	Converter = CreateDefaultSubobject<USpeckleConverterComponent>(FName("Converter"));
	
	KeepCache = true;
	DisplayProgressBar = true;
	ServerUrl = "https://speckle.xyz";
}

// Begin Play function (Receive)
void ASpeckleUnrealManager::BeginPlay()
{
	Super::BeginPlay();

	if(ImportAtRuntime) Receive();
}

// Start the Receive process
void ASpeckleUnrealManager::Receive()
{
	DeleteObjects();

	// Trim parameters
	ServerUrl.TrimEndInline();
	while(ServerUrl.RemoveFromEnd("/")) { }
	StreamID.TrimEndInline();
	ObjectID.TrimEndInline();
	AuthToken.TrimEndInline();

	// Analytics
	FAnalytics::TrackEvent(
		"unknown",
		"unknown",
		"NodeRun",
		TMap<FString, FString> {
			{"name", StaticClass()->GetName() },
			{"worldType", FString::FromInt(GetWorld()->WorldType)}}
	);

	// Clear cache 
	if(!KeepCache && LocalObjectCache.GetObjectRef() != nullptr)
	{
		LocalObjectCache.GetObjectRef()->ConditionalBeginDestroy();
		LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();
	}

	// Probably a duplication
	if(LocalObjectCache.GetObjectRef() == nullptr)
	{
		LocalObjectCache = UMemoryTransport::CreateEmptyMemoryTransport();
	}
	
	// Debug message
	FString Message = FString::Printf(TEXT("Fetching Objects from Speckle Server: %s"), *ServerUrl);
	PrintMessage(Message);

	// Error delegate
	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleError);

	// Complete delegate
	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &ASpeckleUnrealManager::HandleReceive, DisplayProgressBar);

	// Create a server transport
	UServerTransport* ServerTransport = UServerTransport::CreateServerTransport(ServerUrl,StreamID,AuthToken);
	
	// Receive 
	ServerTransport->CopyObjectAndChildren(ObjectID, LocalObjectCache, CompleteDelegate, ErrorDelegate);
}


// Handle Received JSON
void ASpeckleUnrealManager::HandleReceive(TSharedPtr<FJsonObject> RootObject, bool DisplayProgress)
{
	if(RootObject == nullptr) return;

	// Create a Base Deserialization Object Model
	const UBase* Res = USpeckleSerializer::DeserializeBase(RootObject, LocalObjectCache);

	if(IsValid(Res))
	{
		// Start conversion
		Converter->RecursivelyConvertToNative(this, Res, LocalObjectCache, DisplayProgress, Actors);

		// Finished Conversion - show debug message 
		FString Message = FString::Printf(TEXT("Converted %d Actors"), Actors.Num());
		PrintMessage(Message);
	}
	else
	{
		// Object id from Json
		FString Id;

		// Try to get JSON id field
		RootObject->TryGetStringField("id", Id);

		// Debug message for error wrt to id
		FString Message = FString::Printf(TEXT("Failed to deserialise root object: %s"), *Id);

		// Print Message
		HandleError(Message);
	}
}

// Error handler just prints a debug message
void ASpeckleUnrealManager::HandleError(FString& Message)
{
	PrintMessage(Message, true);
}

// Print debug message
void ASpeckleUnrealManager::PrintMessage(FString& Message, bool IsError) const
{
	// Show at Console log: "Error" or just "Log" message (Errors are shown in red in output console)
	if(IsError)
	{
		UE_LOG(LogSpeckle, Error, TEXT("%s"), *Message);
	}
	else
	{
		UE_LOG(LogSpeckle, Log, TEXT("%s"), *Message);
	}

	// Error as a debug message
	FColor Color = IsError? FColor::Red : FColor::Green;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, Color, Message);
}

// Delete Actors containing all the 3D models
void ASpeckleUnrealManager::DeleteObjects()
{
	// If still converting, terminate process
	Converter->FinishConversion();

	// Destroy actors
	for (AActor* a : Actors)
	{
		if(IsValid(a)) a->Destroy();
	}

	Actors.Empty();
}




#undef LOCTEXT_NAMESPACE