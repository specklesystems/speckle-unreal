// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PointCloud.h"

#include "Conversion/ConversionUtils.h"
#include "Transports/Transport.h"

bool UPointCloud::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;
	
	const float ScaleFactor = UConversionUtils::ParseScaleFactor(Units);

	//Parse Points
	{
		TArray<TSharedPtr<FJsonValue>> ObjectPoints = UConversionUtils::CombineChunks(Obj->GetArrayField("points"), ReadTransport);
		
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
		TArray<TSharedPtr<FJsonValue>> ObjectColors = UConversionUtils::CombineChunks(Obj->GetArrayField("colors"), ReadTransport);
		
		Colors.Reserve(ObjectColors.Num());
		for (int32 i = 0; i < ObjectColors.Num(); i += 1) 
		{
			Colors.Add( FColor(ObjectColors[i].Get()->AsNumber()) );
		}
		DynamicProperties.Remove("colors");
	}

	//Parse Sizes
	{
		TArray<TSharedPtr<FJsonValue>> ObjectSizes = UConversionUtils::CombineChunks(Obj->GetArrayField("sizes"), ReadTransport);
		
		Sizes.Reserve(ObjectSizes.Num());
		for (int32 i = 0; i < ObjectSizes.Num(); i += 1) 
		{
			Sizes.Add( ObjectSizes[i].Get()->AsNumber() * ScaleFactor);
		}
		DynamicProperties.Remove("sizes");
	}

	return Points.Num() >= 0;
}

