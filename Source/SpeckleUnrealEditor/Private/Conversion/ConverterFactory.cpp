// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/ConverterFactory.h"

#include "Conversion/SpeckleConverter.h"
#include "ClassViewerModule.h"
#include "InterfaceClassFilter.h"
#include "SpeckleUnrealEditorModule.h"
#include "Kismet2/SClassPickerDialog.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealEditorModule"

UConverterFactory::UConverterFactory(UClass* SClass)
	: UConverterFactory()
{
	SupportedClass = SClass;
	ConverterClass = SClass;
}

UConverterFactory::UConverterFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USpeckleConverter::StaticClass(); //This is not super valid, but it doesn't matter
	
	ConverterClass = nullptr;
}

uint32 UConverterFactory::GetMenuCategories() const
{
	static const uint32 SpeckleCategory = FModuleManager::LoadModuleChecked<FSpeckleUnrealEditorModule>("SpeckleUnrealEditor").GetSpeckleAssetCategory();
	return SpeckleCategory;
}

bool UConverterFactory::ConfigureProperties()
{
	// Null the CurveClass so we can get a clean class
	ConverterClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FInterfaceClassFilter> Filter = MakeShareable(new FInterfaceClassFilter);
	Options.ClassFilter = Filter;

	Filter->InterfaceThatMustBeImplemented = USpeckleConverter::StaticClass();
	Filter->bAllowAbstract = false;
	Filter->ClassPropertyMetaClass = UObject::StaticClass();
	Filter->AllowedClassFilters = TArray<const UClass*> {UObject::StaticClass()};
	const FText TitleText = LOCTEXT("CreateConverterOptions", "Pick Converter Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UObject::StaticClass());

	if ( bPressedOk )
	{
		ConverterClass = ChosenClass;
	}

	return bPressedOk;
}


UObject* UConverterFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                             UObject* Context, FFeedbackContext* Warn)
{
	UObject* NewObjectAsset = nullptr;
	if(ConverterClass != nullptr)
	{
		NewObjectAsset = NewObject<UObject>(InParent, ConverterClass, Name, Flags);
	}
	return NewObjectAsset;
}

 FText UConverterFactory::GetDisplayName() const
 {
 	return LOCTEXT("ConverterFactoryDisplayName", "Speckle Converter");
 }

#undef LOCTEXT_NAMESPACE
