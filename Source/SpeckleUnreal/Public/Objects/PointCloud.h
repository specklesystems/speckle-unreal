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

	UPointCloud() : UBase(TEXT("Objects.Other.Pointcloud")) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FVector> Points;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FColor> Colors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<float> Sizes;

	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;
};
