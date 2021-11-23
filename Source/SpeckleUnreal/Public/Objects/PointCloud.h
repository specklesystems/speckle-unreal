// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"
#include "PointCloud.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UPointCloud : public UBase
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Points;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FColor> Colors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<float> Sizes;

	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;
};
