#pragma once

#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

class UServerTransport;
class ITransport;
class USpeckleConverterComponent;


/**
 * An Actor to handle the receiving of Speckle objects into a level
 */
UCLASS(ClassGroup=(Speckle), AutoCollapseCategories=("Speckle|Conversion"), BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ServerUrl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString StreamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ObjectID;

	// A Personal Access Token can be created from your Speckle Profile page (Treat tokens like passwords, do not share publicly) - Required for receiving private streams
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle", meta=(PasswordField = true))
	FString AuthToken;

	// When true, will call `Receive` on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle", AdvancedDisplay)
	bool ImportAtRuntime;

	// When true, will maintain an in-memory (transient) cache of received speckle objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle", AdvancedDisplay)
	bool KeepCache;
	
	// The Conversion component to convert received speckle objects into native Actors
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpeckleConverterComponent* Converter;
	
	// Used to stagger transport requests, useful when making requests for a large number of child objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle", AdvancedDisplay)
	bool DisplayProgressBar;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Receives specified object from specified Speckle server
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Speckle")
	virtual void Receive();

	// Deletes the Actors created by the previous receive operation
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Speckle")
	virtual void DeleteObjects();
	
	virtual void BeginPlay() override;

	
protected:

	// Cache received Speckle objects
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<ITransport> LocalObjectCache;

	// Array of Actors created by the previous receive operation
	UPROPERTY()
	TArray<AActor*> Actors;

	// Callback when JSON has been received
	virtual void HandleReceive(TSharedPtr<FJsonObject> RootObject, bool DisplayProgress = false);
	
	// Callback when error
	virtual void HandleError(FString& Message);
	
	virtual void PrintMessage(FString& Message, bool IsError = false) const;
};