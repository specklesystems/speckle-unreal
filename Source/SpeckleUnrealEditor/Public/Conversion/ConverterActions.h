// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"


/**
 * Asset action for ISpeckleConverter implementors.
 */
class FConverterActions : public FAssetTypeActions_Base
{
private:
	UClass* SupportedClass;
	uint32 Category;
	
public:
	explicit FConverterActions(UClass* SupportedClass, uint32 Category)
	{
		this->SupportedClass = SupportedClass;
		this->Category = Category;
	}
	
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return SupportedClass->GetDisplayNameText(); }
	virtual UClass* GetSupportedClass() const override { return SupportedClass; }
	virtual FColor GetTypeColor() const override { return FColor(59, 130, 246); }
	virtual bool CanLocalize() const override { return false; }
	
	virtual uint32 GetCategories() override { return Category; }
	
};
