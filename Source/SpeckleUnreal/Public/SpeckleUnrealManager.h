#pragma once

#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

class ITransport;
class USpeckleConverterComponent;


/**
 * An Actor to handle the receiving of Speckle objects into a level
 */
UCLASS(ClassGroup=(Speckle), BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()
	
public:

	// The Conversion component to convert received speckle objects into native Actors
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpeckleConverterComponent* Converter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ServerUrl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString StreamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ObjectID;

	// A Personal Access Token can be created from your Speckle Profile page (Treat tokens like passwords, do not share publicly)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle", meta=(PasswordField = true))
		FString AuthToken;

	// When true, will call `Receive` on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	bool ImportAtRuntime;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealManager();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void Receive();

	// Deletes the Actors created by the previous receive operation
	UFUNCTION(BlueprintCallable, CallInEditor)
	void DeleteObjects();
	
	virtual void BeginPlay() override;

	
protected:

	// Cache received Speckle objects
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<ITransport> LocalObjectCache;

	// Array of Actors created by the previous receive operation
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> Actors;
	
	void HandleReceive(TSharedPtr<FJsonObject> RootObject);
	
	void HandleError(FString& Message);
};