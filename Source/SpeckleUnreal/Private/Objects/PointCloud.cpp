// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PointCloud.h"

#include "SpeckleUnrealManager.h"

bool UPointCloud::Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	if(!Super::Parse(Obj, Manager)) return false;
	
	const float ScaleFactor = Manager->ParseScaleFactor(Units);

	//Parse Points
	{
		TArray<TSharedPtr<FJsonValue>> ObjectPoints = Manager->CombineChunks(Obj->GetArrayField("points"));
		
		Points.Reserve(ObjectPoints.Num() / 3);
		for (int32 i = 2; i < ObjectPoints.Num(); i += 3) 
		{
			Points.Add(FVector
			(
				-ObjectPoints[i - 2].Get()->AsNumber(),
				ObjectPoints[i - 1].Get()->AsNumber(),
				ObjectPoints[i].Get()->AsNumber()
			) * ScaleFactor);
		}
		DynamicProperties.Remove("points");
	}

	
	//Parse Colors
	{
		TArray<TSharedPtr<FJsonValue>> ObjectColors = Manager->CombineChunks(Obj->GetArrayField("colors"));
		
		Colors.Reserve(ObjectColors.Num());
		for (int32 i = 0; i < ObjectColors.Num(); i += 1) 
		{
			Colors.Add( FColor(ObjectColors[i].Get()->AsNumber()) );
		}
		DynamicProperties.Remove("colors");
	}

	//Parse Sizes
	{
		TArray<TSharedPtr<FJsonValue>> ObjectSizes = Manager->CombineChunks(Obj->GetArrayField("sizes"));
		
		Sizes.Reserve(ObjectSizes.Num());
		for (int32 i = 0; i < ObjectSizes.Num(); i += 1) 
		{
			Sizes.Add( ObjectSizes[i].Get()->AsNumber() * ScaleFactor);
		}
		DynamicProperties.Remove("sizes");
	}

	return Points.Num() >= 0;
}

