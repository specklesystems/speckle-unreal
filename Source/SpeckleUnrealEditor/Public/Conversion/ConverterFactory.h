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
 * Factory for ISpeckleConverter classes to appear in the asset creation context menu.
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