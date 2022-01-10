// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"
#include "BuiltElement.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UBuiltElement : public UBase
{
	GENERATED_BODY()
protected:
	static TArray<FString> DisplayValueAliasStrings;
	void AddDisplayValue(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager);
	
public:
	
	UBuiltElement() : UBase(TEXT("Objects.Elements")) {}
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
    TArray<UBase*> DisplayValue;
	
	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;
};
