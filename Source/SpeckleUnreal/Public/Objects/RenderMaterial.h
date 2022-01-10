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

	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override
	{
		Super::Parse(Obj, Manager);
	
		Obj->TryGetStringField("name", Name);
		Obj->TryGetNumberField("opacity", Opacity);
		Obj->TryGetNumberField("metalness", Metalness);
		Obj->TryGetNumberField("roughness", Roughness);
	
		int32 ARGB;
		if(Obj->TryGetNumberField("diffuse", ARGB))
			Diffuse = FColor(ARGB);
		
		if(Obj->TryGetNumberField("emissive", ARGB))
			Emissive = FColor(ARGB);
		
	}

	
};
