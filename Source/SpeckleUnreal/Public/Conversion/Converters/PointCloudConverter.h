// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"

#include "PointCloudConverter.generated.h"

class ULidarPointCloudComponent;
class ALidarPointCloudActor;
class ULidarPointCloud;
class UPointCloud;

/**
 * Converts Speckle Point Cloud objects into LidarPointClouds
 */
UCLASS()
class SPECKLEUNREAL_API UPointCloudConverter :  public UObject, public ISpeckleConverter
{
	GENERATED_BODY()
	
	CONVERTS_SPECKLE_TYPES()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ALidarPointCloudActor> PointCloudActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EComponentMobility::Type> ActorMobility;
	
	UPointCloudConverter();
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>&) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;

	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* PointCloudToNative(const UPointCloud* SpecklePointCloud, UWorld* World);
	
	UFUNCTION(BlueprintCallable)
	virtual UPointCloud* PointCloudToSpeckle(const ULidarPointCloudComponent* Object);

protected:
	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* CreateActor(UWorld* World, ULidarPointCloud* PointCloudData);
};
