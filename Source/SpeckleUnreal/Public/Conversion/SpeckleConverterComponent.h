// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "SpeckleConverterComponent.generated.h"

class ITransport;
class USpeckleConverter;
class ASpeckleUnrealManager;
class UBase;
class ISpeckleConverter;

UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API USpeckleConverterComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	// A lazily initialised mapping of SpeckleType -> converters.
	TMap<TSubclassOf<UBase>, TScriptInterface<ISpeckleConverter>> SpeckleTypeMap;

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
	virtual UBase* ConvertToSpeckle(UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual UObject* ConvertToNative(const UBase* Object, UWorld* World);


	// Converts the given Base and all children into native actors.
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual UPARAM(DisplayName = "RootActor") AActor* RecursivelyConvertToNative(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport> LocalTransport, TArray<AActor*>& OutActors);
	
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual TScriptInterface<ISpeckleConverter> GetConverter(const TSubclassOf<UBase> BaseType);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void CleanUp();
	
	static bool CheckValidConverter(const UObject* Converter, bool LogWarning = true);

protected:

	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void ConvertChildren(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport> LocalTransport, TArray<AActor*>& OutActors);

};


