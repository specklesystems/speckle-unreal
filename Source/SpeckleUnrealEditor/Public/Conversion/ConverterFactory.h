// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

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