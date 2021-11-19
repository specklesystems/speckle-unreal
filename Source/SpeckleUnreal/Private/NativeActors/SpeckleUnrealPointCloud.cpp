// Fill out your copyright notice in the Description page of Project Settings.


#include "NativeActors/SpeckleUnrealPointCloud.h"

#include "LidarPointCloudComponent.h"
#include "Objects/PointCloud.h"

// Sets default values
ASpeckleUnrealPointCloud::ASpeckleUnrealPointCloud() : ALidarPointCloudActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");;
	RootComponent->SetMobility(EComponentMobility::Static);
}

void ASpeckleUnrealPointCloud::SetData_Implementation(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager)
{
	TArray<FLidarPointCloudPoint> LidarPoints;
	
	LidarPoints.Reserve(SpecklePointCloud->Points.Num());

	for(int i = 0; i < SpecklePointCloud->Points.Num(); i++)
	{
		FColor c = SpecklePointCloud->Colors.Num() > i? SpecklePointCloud->Colors[i] : FColor::White;
		FLidarPointCloudPoint p = FLidarPointCloudPoint(SpecklePointCloud->Points[i], c, true, 0);
		LidarPoints.Add(p);
	}
	
	ULidarPointCloud* PointCloud = NewObject<ULidarPointCloud>();
	
	PointCloud->Initialize(FBox(SpecklePointCloud->Points));

	PointCloud->InsertPoints(LidarPoints, ELidarPointCloudDuplicateHandling::Ignore, false, FVector::ZeroVector);

	PointCloud->RefreshBounds();

	this->SetPointCloud(PointCloud);
}

