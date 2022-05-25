// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/SlowTask.h"
#include "SpeckleConverterComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FActorPredicate, const AActor*, Actor, bool&, OutShouldConvert); 

class ITransport;
class ISpeckleConverter;
class UBase;
class UAggregateConverter;

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
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion|ToNative")
	UPARAM(DisplayName = "RootActor") AActor* RecursivelyConvertToNative(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, bool DisplayProgressBar, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion|ToNative")
	virtual void FinishConversion();

	// Converts the given Base and all children into native actors.
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion|ToSpeckle")
	virtual UBase* RecursivelyConvertToSpeckle(const TArray<AActor*>& RootActors, FActorPredicate& Predicate);

	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion|ToSpeckle")
	virtual void RecurseTreeToSpeckle(const AActor* RootActor, FActorPredicate& Predicate, TArray<UBase*>& OutConverted);
	
	

protected:

	virtual AActor* RecursivelyConvertToNative_Internal(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, FSlowTask* Task, TArray<AActor*>& OutActors);

	
	virtual void ConvertChildren(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, FSlowTask* Task, TArray<AActor*>& OutActors);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Conversion")
	virtual void AttachConvertedToOwner(AActor* AOwner, const UBase* Base, UObject* Converted);
};


