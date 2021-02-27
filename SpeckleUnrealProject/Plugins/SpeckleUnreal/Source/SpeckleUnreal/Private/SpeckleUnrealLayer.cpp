// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleUnrealLayer.h"

ASpeckleUnrealLayer::ASpeckleUnrealLayer()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Scene;
}

void ASpeckleUnrealLayer::Init(FString NewLayerName, int32 NewStartIndex, int32 NewObjectCount)
{
	SetActorLabel(NewLayerName);

	LayerName = NewLayerName;
	LayerColor = FLinearColor(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), 1);
	StartIndex = NewStartIndex;
	ObjectCount = NewObjectCount;
}