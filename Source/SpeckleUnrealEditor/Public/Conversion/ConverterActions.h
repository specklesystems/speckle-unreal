// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AssetTypeActions_Base.h"


/**
 * 
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
