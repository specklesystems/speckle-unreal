// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"

#include "PointCloudConverter.generated.h"

class ULidarPointCloudComponent;
class ALidarPointCloudActor;
class ULidarPointCloud;
class UPointCloud;

UCLASS()
class SPECKLEUNREAL_API UPointCloudConverter :  public UObject, public ISpeckleConverter
{
	GENERATED_BODY()
	
	CONVERTS_SPECKLE_TYPES()
	
protected:
	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* CreateActor(const ASpeckleUnrealManager* Manager, ULidarPointCloud* PointCloudData);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ALidarPointCloudActor> PointCloudActorType;
	
	UPointCloudConverter();
	
	virtual AActor* ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager) override;
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager) override;

	UFUNCTION(BlueprintCallable)
	virtual ALidarPointCloudActor* PointCloudToNative(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager);
	
	UFUNCTION(BlueprintCallable)
	virtual UPointCloud* PointCloudToSpeckle(const ULidarPointCloudComponent* Object, ASpeckleUnrealManager* Manager);
};
