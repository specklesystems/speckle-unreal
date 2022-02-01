// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"

#include "BlockInstance.generated.h"

class UBase;
/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UBlockInstance : public UBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<UBase*> Geometry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FMatrix Transform;
	
	UBlockInstance() : UBase(TEXT("Objects.Other.BlockInstance")) {}

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;

};
