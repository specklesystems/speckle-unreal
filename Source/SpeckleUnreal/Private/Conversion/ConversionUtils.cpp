// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/ConversionUtils.h"

FMatrix UConversionUtils::TransformToNative(const TArray<float>& TransformData)
{
	if(TransformData.Num() != 16) return FMatrix::Identity;
	
	FMatrix TransformMatrix;
		
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			TransformMatrix.M[Row][Col] = TransformData[Row * 4 + Col];
		}
	
	TransformMatrix = TransformMatrix.GetTransposed();

	return TransformMatrix;	
}

float UConversionUtils::GetUnitsScaleFactorF(const FString& Units, const float WorldToCentimeters)
{
	static const auto ParseUnits = [](const FString& LUnits) -> float
	{
		if (LUnits == "millimeters" || LUnits == "millimeter" || LUnits == "millimetres" || LUnits == "millimetre" || LUnits == "mm")
			return 0.1;
		if (LUnits == "centimeters" || LUnits == "centimeter" ||LUnits == "centimetres" || LUnits == "centimetre" || LUnits == "cm")
			return 1;
		if (LUnits == "meters" || LUnits == "meter" || LUnits == "metres" || LUnits == "metre" || LUnits == "m")
			return 100;
		if (LUnits == "kilometers" || LUnits == "kilometres" || LUnits == "km")
			return 100000;

		if (LUnits == "inches" || LUnits == "inch" || LUnits == "in")
			return 2.54;
		if (LUnits == "feet" || LUnits == "foot" || LUnits == "ft")
			return 30.48;
		if (LUnits == "yards" || LUnits == "yard"|| LUnits == "yd")
			return 91.44;
		if (LUnits == "miles" || LUnits == "mile" || LUnits == "mi")
			return 160934.4;

		return 100;
	};

	return ParseUnits(Units.ToLower()) * WorldToCentimeters;
}

float UConversionUtils::GetUnitsScaleFactor(const FString& Units, const UWorld* World)
{
	int32 WorldToCentimeters;
	ensureAlways(TryGetWorldUnits(World, WorldToCentimeters));
	
	return GetUnitsScaleFactorF(Units, WorldToCentimeters);
}

bool UConversionUtils::TryGetWorldUnits(const UWorld* World, int32& OutWorldToCentimeters)
{
	OutWorldToCentimeters = 1;
	if(!IsValid(World)) return false;
	
	const AWorldSettings* Settings = World->GetWorldSettings();
	if(!IsValid(Settings)) return false;

	OutWorldToCentimeters = Settings->WorldToMeters / 10.0;
	return true;
}
