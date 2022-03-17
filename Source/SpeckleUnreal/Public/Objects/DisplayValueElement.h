// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"
#include "DisplayValueElement.generated.h"


class UMesh;

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UDisplayValueElement : public UBase
{
	GENERATED_BODY()
protected:
	static TArray<FString> DisplayValueAliasStrings;
	bool AddDisplayValue(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);
	
public:
	
	UDisplayValueElement() : UBase(TEXT("Objects.BuiltElements")) {}
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
    TArray<UMesh*> DisplayValue;
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
};
