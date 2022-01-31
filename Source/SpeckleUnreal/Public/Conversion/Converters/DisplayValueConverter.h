// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Conversion/SpeckleConverter.h"
#include "DisplayValueConverter.generated.h"

class UStaticMeshConverter;
class UDisplayValueElement;
/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UDisplayValueConverter : public UObject, public ISpeckleConverter
{
	GENERATED_BODY()
	
	CONVERTS_SPECKLE_TYPES()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshConverter* MeshConverter;

	UDisplayValueConverter();
	
	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;

	
};
