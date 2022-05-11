// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSpeckleUnrealEditorModule : public IModuleInterface
{
	
#if WITH_EDITORONLY_DATA
protected:
	uint32 SpeckleAssetCategoryBit = 0;
#endif
	
#if WITH_EDITOR
public:
	virtual uint32 GetSpeckleAssetCategory() const;
#endif
	
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
