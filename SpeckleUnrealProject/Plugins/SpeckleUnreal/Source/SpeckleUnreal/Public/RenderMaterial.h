// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RenderMaterial.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API URenderMaterial : public UObject
{
	GENERATED_BODY()

	public:

		UPROPERTY()
		FString ObjectID;
	
		UPROPERTY()
		FString Name;
		
		UPROPERTY()
		double Opacity = 1;
		
		UPROPERTY()
		double Metalness = 0;
		
		UPROPERTY()
		double Roughness = 1;
		
		UPROPERTY()
		FColor Diffuse = FColor{221,221,221};
		
		UPROPERTY()
		FColor Emissive = FColor::Black;

		

	
};
