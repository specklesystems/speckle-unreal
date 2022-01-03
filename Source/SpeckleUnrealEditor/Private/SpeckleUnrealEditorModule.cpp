// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpeckleUnrealEditorModule.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"

#include "Conversion/ConverterActions.h"
#include "Conversion/ConverterFactories.h"
#include "Conversion/ConverterFactory.h"

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

		// Finds all class definitions : UConverterFactory and creates a FConverterActions for their supported class.
		// See UAssetToolsImpl::GetNewAssetFactories() for reference
		for (TObjectIterator<UClass> It; It; ++It)
		{
			const UClass* Class = *It;
			if (Class->IsChildOf(UConverterFactory::StaticClass()) &&
				!Class->HasAnyClassFlags(CLASS_Abstract))
			{
				const UConverterFactory* Factory = Class->GetDefaultObject<UConverterFactory>();

				if (Factory->ShouldShowInNewMenu() &&
					ensure(!Factory->GetDisplayName().IsEmpty()))
				{
					AssetTools.RegisterAssetTypeActions(MakeShareable(new FConverterActions(Factory->SupportedClass, SpeckleAssetCategoryBit)));
				}
			}
		}
		
		
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