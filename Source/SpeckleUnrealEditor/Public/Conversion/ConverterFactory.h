// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Conversion/Converters/BlockConverter.h"
#include "Conversion/Converters/DisplayValueConverter.h"
#include "Conversion/Converters/ProceduralMeshConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/RenderMaterialConverter.h"

#include "ConverterFactory.generated.h"

/**
 * This class is designed to reduce the boiler plate required to define a UFactory for ISpeckleTypeConverter types.
 * 
 * For each ISpeckleTypeConverter we want to appear in the "Create Advanced Asset" context menu
 * A class definition : UConverterFactory should be created (see below examples)
 *
 * A FConverterAction : FAssetTypeActions_Base instance will automatically be created by
 * FSpeckleUnrealEditorModule::StartupModule for each class definition : UConverterFactory.
 * So we don't need to worry about manually registering the SupportedClass types with FAssetToolsModule.
 */
UCLASS()
class SPECKLEUNREALEDITOR_API UConverterFactory : public UFactory
{
	GENERATED_BODY()
protected:
	explicit UConverterFactory(UClass* SupportedClass);
public:
	UPROPERTY(EditAnywhere, Category="Speckle|Factories", meta = (MustImplement = USpeckleConverter))
	UClass* ConverterClass;
	
	UConverterFactory();
	
	virtual uint32 GetMenuCategories() const override;

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;

};

//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API UStaticMeshConverterFactory : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	UStaticMeshConverterFactory() : Super( UStaticMeshConverter::StaticClass() ) { }
// 	virtual bool ConfigureProperties() override { return true; }
// 	virtual bool ShouldShowInNewMenu() const override { return false; }
// };
//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API UProceduralMeshConverterFactory  : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	UProceduralMeshConverterFactory() : Super( UProceduralMeshConverter::StaticClass() ) { } 
// 	virtual bool ConfigureProperties() override { return true; }
// 	virtual bool ShouldShowInNewMenu() const override { return false; }
// };
//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API UPointCloudConverterFactory  : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	UPointCloudConverterFactory() : Super( UPointCloudConverter::StaticClass() ) { }
// 	virtual bool ConfigureProperties() override { return true; }
// 	virtual bool ShouldShowInNewMenu() const override { return false; }
// }; 
//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API UDisplayValueConverterFactory  : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	UDisplayValueConverterFactory() : Super( UDisplayValueConverter::StaticClass() ) { }
// 	virtual bool ConfigureProperties() override { return true; }
// 	virtual bool ShouldShowInNewMenu() const override { return false; }
// }; 
//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API UBlockConverterFactory  : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	UBlockConverterFactory() : Super( UBlockConverter::StaticClass() ) { }
// 	virtual bool ConfigureProperties() override { return true; }
// 	virtual bool ShouldShowInNewMenu() const override { return false; }
// };
//
// UCLASS() 
// class SPECKLEUNREALEDITOR_API URenderMaterialConverterFactory  : public UConverterFactory
// { 
// 	GENERATED_BODY() 
// public: 
// 	URenderMaterialConverterFactory() : Super( URenderMaterialConverter::StaticClass() ) { } 
// 	virtual bool ConfigureProperties() override { return true; }
//     virtual bool ShouldShowInNewMenu() const override { return false; }
// };  