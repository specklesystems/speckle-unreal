// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LidarPointCloudActor.h"
#include "Interfaces/SpecklePointCloud.h"

#include "SpeckleUnrealPointCloud.generated.h"

UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealPointCloud : public ALidarPointCloudActor, public ISpecklePointCloud
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	ASpeckleUnrealPointCloud();

	virtual void SetData_Implementation(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager) override;
	
};
