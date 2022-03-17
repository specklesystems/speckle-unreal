// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/Converters/PointCloudConverter.h"

#include "LidarPointCloudActor.h"
#include "LidarPointCloudComponent.h"
#include "Objects/Geometry/PointCloud.h"


UPointCloudConverter::UPointCloudConverter()
{
	SpeckleTypes.Add(UPointCloud::StaticClass());
	
	PointCloudActorType = ALidarPointCloudActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}


UObject* UPointCloudConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>&)
{
	const UPointCloud* p = Cast<UPointCloud>(SpeckleBase);
	
	if(p == nullptr) return nullptr;
	
	return PointCloudToNative(p, World);
}


ALidarPointCloudActor* UPointCloudConverter::PointCloudToNative(const UPointCloud* SpecklePointCloud, UWorld* World)
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

	return CreateActor(World, PointCloud);
	
}


ALidarPointCloudActor* UPointCloudConverter::CreateActor(UWorld* World, ULidarPointCloud* PointCloudData)
{
	ALidarPointCloudActor* Actor = World->SpawnActor<ALidarPointCloudActor>(PointCloudActorType);
	Actor->SetPointCloud(PointCloudData);
	Actor->GetRootComponent()->SetMobility(ActorMobility);
	return Actor;
}


UBase* UPointCloudConverter::ConvertToSpeckle_Implementation(const UObject* Object)
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
	
	return PointCloudToSpeckle(P);
}


UPointCloud* UPointCloudConverter::PointCloudToSpeckle(const ULidarPointCloudComponent* Object)
{
	return nullptr; //TODO implement ToSpeckle function
}


