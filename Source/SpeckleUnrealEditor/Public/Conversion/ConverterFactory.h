// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "ConverterFactory.generated.h"

/**
 * This class is designed to reduce the boiler plate required to define a UFactory for ISpeckleTypeConverter types.
 * 
 */

UCLASS(abstract)
class SPECKLEUNREALEDITOR_API UConverterFactory : public UFactory
{
	GENERATED_BODY()
	
protected:
	explicit UConverterFactory(UClass* SupportedClass);
	
public:
	UConverterFactory() : Super() {}
	
	virtual uint32 GetMenuCategories() const override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};




