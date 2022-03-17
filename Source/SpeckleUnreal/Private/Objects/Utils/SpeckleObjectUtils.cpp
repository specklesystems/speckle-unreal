// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Transports/Transport.h"


TArray<TSharedPtr<FJsonValue>> USpeckleObjectUtils::CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField, const TScriptInterface<ITransport> Transport)
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

bool USpeckleObjectUtils::ResolveReference(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, TSharedPtr<FJsonObject>& OutObject)
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

float USpeckleObjectUtils::ParseScaleFactor(const FString& UnitsString)
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

bool USpeckleObjectUtils::TryParseTransform(const TSharedPtr<FJsonObject> SpeckleObject, FMatrix& OutMatrix)
{
	const TSharedPtr<FJsonObject>* TransformObject;
	const TArray<TSharedPtr<FJsonValue>>* TransformData;
		
	if(SpeckleObject->TryGetArrayField("transform", TransformData)) //Handle transform as array
	{ }
	else if(SpeckleObject->TryGetObjectField("transform", TransformObject)
		&& (*TransformObject)->TryGetArrayField("value", TransformData)) //Handle transform as object
	{ }
	else return false;
		
	FMatrix TransformMatrix;
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			TransformMatrix.M[Row][Col] = TransformData->operator[](Row * 4 + Col)->AsNumber();
		}
	OutMatrix = TransformMatrix.GetTransposed();
	return true;
}
