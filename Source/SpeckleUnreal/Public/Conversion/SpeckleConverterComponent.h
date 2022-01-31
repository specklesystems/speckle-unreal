// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeckleConverterComponent.generated.h"

class USpeckleTypeConverter;
class ASpeckleUnrealManager;
class UBase;
class ISpeckleTypeConverter;

/**
 * This component contains modular conversion functions for converting Speckle Objects <--> Native Unreal Objects.
 */
UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API USpeckleConverterComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	// A lazily initialised mapping of SpeckleType -> converters.
	TMap<TSubclassOf<UBase>, TScriptInterface<ISpeckleTypeConverter>> SpeckleTypeMap;

public:
	
	// Array of converters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle|Conversion")
	TArray<UObject*> SpeckleConverters;
	
	// Sets default values for this component's properties
	USpeckleConverterComponent();

	// Validates changes to SpeckleConverters, Should be called after modifying SpeckleConverters
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void OnConvertersChangeHandler();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	UBase* ConvertToSpeckle(UObject* Object);

	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	AActor* ConvertToNative(const UBase* Object, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	TScriptInterface<ISpeckleTypeConverter> GetConverter(const TSubclassOf<UBase> BaseType);
};


