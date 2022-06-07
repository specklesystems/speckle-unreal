// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"
#include "Plane.generated.h"

/**
 * 
 */
UCLASS(meta=(ScriptName="Plane (Speckle.Objects)"))
class SPECKLEUNREAL_API UPlane : public UBase
{
	GENERATED_BODY()


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FVector Origin, Normal, XDir, YDir;
	

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
	
};
