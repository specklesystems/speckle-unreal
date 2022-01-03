// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conversion/ConverterFactory.h"
#include "Conversion/TypeConverters/ProceduralMeshConverter.h"
#include "Conversion/TypeConverters/StaticMeshConverter.h"
#include "Conversion/TypeConverters/PointCloudConverter.h"

#include "ConverterFactories.generated.h"


// What Unreal Requires:
// For each Converter we want to appear in the "Create Advanced Asset" context menu,
// 1) We need to declare a UFactory class definition
// 2) We need to register a FAssetTypeActions_Base instance with in FAssetToolsModule

// How we (Speckle) have made that easier:
// A FConverterAction : FAssetTypeActions_Base instance will automatically be created by FSpeckleUnrealEditorModule::StartupModule for each class definition : UConverterFactory
// To add a just declare a class definition : UConverterFactory like the few below (they can be declared in any editor module (untested)) 


UCLASS() 
class SPECKLEUNREALEDITOR_API UStaticMeshConverterFactory : public UConverterFactory
{ 
	GENERATED_BODY() 
public: 
	UStaticMeshConverterFactory() : Super( UStaticMeshConverter::StaticClass() ) { } 
};

UCLASS() 
class SPECKLEUNREALEDITOR_API UProceduralMeshConverterFactory  : public UConverterFactory
{ 
	GENERATED_BODY() 
public: 
	UProceduralMeshConverterFactory() : Super( UProceduralMeshConverter::StaticClass() ) { } 
};

UCLASS() 
class SPECKLEUNREALEDITOR_API UPointCloudConverterFactory  : public UConverterFactory
{ 
	GENERATED_BODY() 
public: 
	UPointCloudConverterFactory() : Super( UPointCloudConverter::StaticClass() ) { } 
}; 
