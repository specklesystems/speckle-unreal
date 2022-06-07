// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "RenderMaterial.generated.h"

/**
 * 
 */
UCLASS(meta=(ScriptName="Render Material (Speckle.Objects)"))
class SPECKLEUNREAL_API URenderMaterial : public UBase
{
	GENERATED_BODY()

public:
	
	URenderMaterial() : UBase(TEXT("Objects.Other.RenderMaterial")) {}

	UPROPERTY()
	FString Name;
	
	UPROPERTY()
	double Opacity = 1;
	
	UPROPERTY()
	double Metalness = 0;
	
	UPROPERTY()
	double Roughness = 1;
	
	UPROPERTY()
	FLinearColor Diffuse = FColor{221,221,221};
	
	UPROPERTY()
	FLinearColor Emissive = FLinearColor::Black;

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override
	{
		if(!Super::Parse(Obj, ReadTransport)) return false;
	
		if(Obj->TryGetStringField("name", Name)) DynamicProperties.Remove("name");
		if(Obj->TryGetNumberField("opacity", Opacity)) DynamicProperties.Remove("opacity");
		if(Obj->TryGetNumberField("metalness", Metalness)) DynamicProperties.Remove("metalness");
		if(Obj->TryGetNumberField("roughness", Roughness)) DynamicProperties.Remove("roughness");

		bool IsValid = false;
		
		int32 ARGB;
		if(Obj->TryGetNumberField("diffuse", ARGB))
		{
			Diffuse = FColor(ARGB);
			DynamicProperties.Remove("diffuse");
			IsValid = true;
		}
		
		if(Obj->TryGetNumberField("emissive", ARGB))
		{
			Emissive = FColor(ARGB);
			DynamicProperties.Remove("emissive");
		}

		return IsValid;
	}

	
};
