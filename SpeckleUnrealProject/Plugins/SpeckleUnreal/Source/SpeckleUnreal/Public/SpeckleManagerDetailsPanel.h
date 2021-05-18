// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"
#include "Input/Reply.h"
#include "SCustomCombo.h"
#include "SpeckleUnrealManager.h"


class SPECKLEUNREAL_API SpeckleManagerDetailsPanel : public IDetailCustomization
{

private:
	/* Contains references to all selected objects inside in the viewport */
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	
public:
	
	/* Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();
 
	/* IDetalCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
 
	FReply FecthBtnListener();
	FReply ImportObjBtnListener();

	TSharedPtr<SCustomCombo> CustomComboBox;

	ASpeckleUnrealManager* SpeckleManager; 
};
