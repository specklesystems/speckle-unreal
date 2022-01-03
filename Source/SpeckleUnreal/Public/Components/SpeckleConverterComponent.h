// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeckleConverterComponent.generated.h"

class ASpeckleUnrealManager;
class UBase;
class ISpeckleTypeConverter;

UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API USpeckleConverterComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Speckle|Convert")
	TMap<TSubclassOf<UBase>, UObject*> SpeckleConverters;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Speckle|Convert", meta = (MustImplement = "SpeckleTypeConverter"))
	//TList<TSubclassOf<UObject>> SpeckleConverters;
	
	// Sets default values for this component's properties
	USpeckleConverterComponent();
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Convert")
	UBase* ConvertToSpeckle(UObject* Object);
	UFUNCTION(BlueprintPure, Category="Speckle|Convert")
	bool CanConvertToSpeckle(const UObject* Object) const;

	UFUNCTION(BlueprintCallable, Category="Speckle|Convert")
	AActor* ConvertToNative(const UBase* Object, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintPure, Category="Speckle|Convert")
	bool CanConvertToNative(const TSubclassOf<UBase> Type) const;
};


