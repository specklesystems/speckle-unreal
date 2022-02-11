// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/ConversionUtils.h"
#include "Transports/Transport.h"


TArray<TSharedPtr<FJsonValue>> UConversionUtils::CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField, const TScriptInterface<ITransport> Transport)
{
	TArray<TSharedPtr<FJsonValue>> ObjectPoints;
		
	for(int32 i = 0; i < ArrayField.Num(); i++) 
	{
		FString Index;
		if(ArrayField[i]->AsObject()->TryGetStringField("referencedId", Index))
		{
			const auto Chunk = Transport->GetSpeckleObject(Index)->GetArrayField("data");;
			ObjectPoints.Append(Chunk);
		}
		else
		{
			return ArrayField; //Array was never chunked to begin with
		}
	}
	return ObjectPoints;
}

bool UConversionUtils::ResolveReference(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, TSharedPtr<FJsonObject>& OutObject)
{
	FString SpeckleType;	
	FString ReferenceID;

	if (Object->TryGetStringField("speckle_type", SpeckleType)
		&& SpeckleType == "reference"
		&& Object->TryGetStringField("referencedId",ReferenceID))
	{
		OutObject = Transport->GetSpeckleObject(ReferenceID); //TODO Consider handle cases where transport doesn't have the object
		return OutObject != nullptr;
	}
	return false;
}

float UConversionUtils::ParseScaleFactor(const FString& UnitsString)
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

	return ParseUnits(UnitsString.ToLower()); // * WorldToCentimeters; //TODO take into account world units
}