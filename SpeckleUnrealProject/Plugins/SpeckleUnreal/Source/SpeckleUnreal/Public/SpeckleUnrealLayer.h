// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpeckleUnrealLayer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API USpeckleUnrealLayer : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		FString LayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		FLinearColor LayerColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		int32 StartIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		int32 ObjectCount;

	USpeckleUnrealLayer();

	void Init(FString NewLayerName, int32 NewStartIndex, int32 NewObjectCount);
};
