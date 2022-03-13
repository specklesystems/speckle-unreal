// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Converters/AggregateConverter.h"
#include "SpeckleConverterComponent.generated.h"

class ITransport;
class ISpeckleConverter;
class UBase;


/**
 * An Actor Component for encapsulating recursive conversion of Speckle Objects
 */
UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API USpeckleConverterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Array of converters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle|Conversion")
	UAggregateConverter* SpeckleConverter;
	
	// Sets default values for this component's properties
	USpeckleConverterComponent();
	
	// Converts the given Base and all children into native actors.
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual UPARAM(DisplayName = "RootActor") AActor* RecursivelyConvertToNative(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, TArray<AActor*>& OutActors);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void CleanUp();

protected:

	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void ConvertChildren(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, TArray<AActor*>& OutActors);

};


