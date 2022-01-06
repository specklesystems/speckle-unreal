// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/Converters/PointCloudConverter.h"

#include "LidarPointCloudActor.h"
#include "LidarPointCloudComponent.h"
#include "Objects/PointCloud.h"


UPointCloudConverter::UPointCloudConverter()
{
	SpeckleTypes.Add("Objects.Geometry.Pointcloud");
}


AActor* UPointCloudConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager)
{
	const UPointCloud* P = Cast<UPointCloud>(SpeckleBase);
	
	if(P == nullptr) return nullptr;
	
	return PointCloudToNative(P, Manager);
}


ALidarPointCloudActor* UPointCloudConverter::PointCloudToNative(const UPointCloud* SpecklePointCloud, ASpeckleUnrealManager* Manager)
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
	
	PointCloud->CenterPoints();
	PointCloud->RefreshBounds();

	return CreateActor(PointCloud);
	
}


ALidarPointCloudActor* UPointCloudConverter::CreateActor(ULidarPointCloud* PointCloudData)
{
	ALidarPointCloudActor* Actor = GetWorld()->SpawnActor<ALidarPointCloudActor>(ALidarPointCloudActor::StaticClass());
	Actor->SetPointCloud(PointCloudData);
	return Actor;
}


UBase* UPointCloudConverter::ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager)
{
	const ULidarPointCloudComponent* P = Cast<ULidarPointCloudComponent>(Object);

	if(P == nullptr)
	{
		const AActor* A = Cast<AActor>(Object);
		if(A != nullptr)
		{
			P = A->FindComponentByClass<ULidarPointCloudComponent>();
		}
	}
	if(P == nullptr) return nullptr;
	
	return PointCloudToSpeckle(P, Manager);
}


UPointCloud* UPointCloudConverter::PointCloudToSpeckle(const ULidarPointCloudComponent* Object, ASpeckleUnrealManager* Manager)
{
	return nullptr; //TODO implement ToSpeckle function
}


