// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Base.h"

#include "DisplayValueElement.generated.h"


class UMesh;

/**
 * 
 */
UCLASS(meta=(ScriptName="Display Value Element (Speckle.Objects)"))
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
