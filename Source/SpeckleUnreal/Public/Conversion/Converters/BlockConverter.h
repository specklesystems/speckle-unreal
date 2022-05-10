// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Conversion/SpeckleConverter.h"

#include "BlockConverter.generated.h"

class UBlockInstance;

/**
 *  Converts Speckle Block Instance objects empty native actors with transform.
 *  The Block Definition can then be converted by other converters
 */
UCLASS()
class SPECKLEUNREAL_API UBlockConverter : public UObject, public ISpeckleConverter
{
	GENERATED_BODY()
	
	CONVERTS_SPECKLE_TYPES()
public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TSubclassOf<AActor> BlockInstanceActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TEnumAsByte<EComponentMobility::Type> ActorMobility;
	
	UBlockConverter();
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>& AvailableConverters) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;

	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual AActor* BlockToNative(const UBlockInstance* Block, UWorld* World);
	
protected:
	virtual AActor* CreateEmptyActor(UWorld* World, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
};
