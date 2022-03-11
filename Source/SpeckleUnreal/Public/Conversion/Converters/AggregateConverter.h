// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "UObject/Object.h"
#include "AggregateConverter.generated.h"

/**
 * 
 */
UCLASS()

class SPECKLEUNREAL_API UAggregateConverter : public UObject, public ISpeckleConverter
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
	UAggregateConverter();

	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual UObject* ConvertToNativeInternal(const UBase* Object, UWorld* World);
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>&) override;

	virtual bool CanConvertToNative_Implementation(TSubclassOf<UBase> BaseType) override;

	
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;

	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual TScriptInterface<ISpeckleConverter> GetConverter(const TSubclassOf<UBase> BaseType);
	
	static bool CheckValidConverter(const UObject* Converter, bool LogWarning = true);

	
	void CleanUpInternal();
	virtual void CleanUp_Implementation() override;

	// Validates changes to SpeckleConverters, Should be called after modifying SpeckleConverters
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void OnConvertersChangeHandler();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
