// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LidarPointCloudComponent.h"
#include "Conversion/SpeckleTypeConverter.h"

#include "PointCloudConverter.generated.h"

class ALidarPointCloudActor;
class ULidarPointCloud;
class UPointCloud;

UCLASS()
class SPECKLEUNREAL_API UPointCloudConverter : public UObject, public ISpeckleTypeConverter
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* CreateActor(ULidarPointCloud* PointCloudData);
	
public:
	

	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;

	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* PointCloudToNative(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UPointCloud* PointCloudToSpeckle(const ULidarPointCloudComponent* Object, ASpeckleUnrealManager* Manager);
};
