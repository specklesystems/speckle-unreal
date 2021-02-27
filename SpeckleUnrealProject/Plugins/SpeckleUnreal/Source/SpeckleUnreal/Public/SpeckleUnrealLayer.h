// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpeckleUnrealLayer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealLayer : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		FString LayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		FLinearColor LayerColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		int32 StartIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle")
		int32 ObjectCount;

	ASpeckleUnrealLayer();

	void Init(FString NewLayerName, int32 NewStartIndex, int32 NewObjectCount);
};
