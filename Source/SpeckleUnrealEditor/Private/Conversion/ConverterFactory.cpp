// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/ConverterFactory.h"

#include "SpeckleUnrealEditorModule.h"


UConverterFactory::UConverterFactory(UClass* SupportedClass)
{
	bCreateNew = true;
	bEditAfterNew = true;
	this->SupportedClass = SupportedClass;
}

uint32 UConverterFactory::GetMenuCategories() const
{
	static const uint32 SpeckleCategory = FModuleManager::LoadModuleChecked<FSpeckleUnrealEditorModule>("SpeckleUnrealEditor").GetSpeckleAssetCategory();
	return SpeckleCategory;
}


UObject* UConverterFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UObject* NewObjectAsset = NewObject<UObject>(InParent, Class, Name, Flags);
	return NewObjectAsset;
}

