// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

class UServerTransport;
class ITransport;
class USpeckleConverterComponent;
class UReceiveSelectionComponent;
class FJsonObject;

/**
 * An Actor to handle the receiving of Speckle objects into a level
 */
UCLASS(ClassGroup=(Speckle), AutoCollapseCategories=("Speckle|Conversion"), BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category="Speckle", BlueprintReadWrite)
	UReceiveSelectionComponent* ReceiveSelection;
	
	// When true, will call `Receive` on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", AdvancedDisplay)
	bool ImportAtRuntime;

	// When true, will maintain an in-memory (transient) cache of received speckle objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", AdvancedDisplay)
	bool KeepCache;
	
	// The Conversion component to convert received speckle objects into native Actors
	UPROPERTY(VisibleAnywhere, Category="Speckle", BlueprintReadWrite)
	USpeckleConverterComponent* Converter;
	
	// When true, will display an editor progress bar while receiving objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", AdvancedDisplay)
	bool DisplayProgressBar;
	
	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Receives specified object from specified Speckle server
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Speckle", meta=(DisplayPriority=0))
	virtual void Receive();

	// Deletes the Actors created by the previous receive operation
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Speckle" , meta=(DisplayAfter="Receive"))
	virtual void DeleteObjects();

	
	virtual void BeginPlay() override;

	
protected:

	// Cache received Speckle objects
	UPROPERTY(BlueprintReadWrite, Category="Speckle")
	TScriptInterface<ITransport> LocalObjectCache;

	// Array of Actors created by the previous receive operation
	UPROPERTY(BlueprintReadWrite, Category="Speckle")
	TArray<AActor*> Actors;

	// Callback when JSON has been received
	virtual void HandleReceive(TSharedPtr<FJsonObject> RootObject, bool DisplayProgress = false);
	
	// Callback when error
	virtual void HandleError(FString& Message);
	
	virtual void PrintMessage(FString& Message, bool IsError = false) const;
};