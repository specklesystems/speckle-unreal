// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSpeckleUnrealEditorModule : public IModuleInterface
{

#if WITH_EDITOR
protected:
	uint32 SpeckleAssetCategoryBit = 0;
public:
	virtual uint32 GetSpeckleAssetCategory() const;
#endif
	
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
