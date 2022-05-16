// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpeckleUnreal.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"

void FSpeckleUnrealModule::StartupModule()
{
	// This code will execute after your module is loaded into memory.
	// The exact timing is specified in the .uplugin file per-module
}

void FSpeckleUnrealModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpeckleUnrealModule, SpeckleUnreal)