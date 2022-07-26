// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "SpeckleUnrealEditorModule.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"

#include "Conversion/ConverterActions.h"
#include "Conversion/SpeckleConverter.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealEditorModule"

uint32 FSpeckleUnrealEditorModule::GetSpeckleAssetCategory() const
{
	return SpeckleAssetCategoryBit;
}

void FSpeckleUnrealEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	if (GIsEditor)
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		// Register Speckle Category
		SpeckleAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Speckle")), LOCTEXT("SpeckleCategoryText","Speckle"));

		
		// See UAssetToolsImpl::GetNewAssetFactories() for reference
		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* Class = *It;

			// Create FConverterActions for USpeckleConverter types
			if ( Class->ImplementsInterface(USpeckleConverter::StaticClass())
				&& !Class->HasAnyClassFlags(CLASS_Abstract))
			{
				AssetTools.RegisterAssetTypeActions(MakeShareable(new FConverterActions(Class, SpeckleAssetCategoryBit)));
			}
		}

		AssetTools.RegisterAssetTypeActions(MakeShareable(new FConverterActions(USpeckleConverter::StaticClass(), SpeckleAssetCategoryBit)));

		
	}
#endif
}

void FSpeckleUnrealEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpeckleUnrealEditorModule, SpeckleUnrealEditor)