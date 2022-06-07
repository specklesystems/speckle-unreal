// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mesh.h"
#include "Objects/Base.h"
#include "Box.generated.h"

class UPlane;
/**
 * 
 */
UCLASS(meta=(ScriptName="Box (Speckle.Objects)"))
class SPECKLEUNREAL_API UBoxx : public UBase
{
	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	UPlane* BasePlane;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FVector2f XSize, YSize;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	float Area;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	float Volume;
	

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;

	UFUNCTION(BlueprintCallable)
	virtual UMesh* ToMesh(UObject* Outer = GetTransientPackage()) const;
};
