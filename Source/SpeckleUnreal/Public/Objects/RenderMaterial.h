// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "RenderMaterial.generated.h"

/**
 * 
 */
UCLASS()
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

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override
	{
		if(!Super::Parse(Obj, Manager)) return false;
	
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
