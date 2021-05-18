// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h" 

class SPECKLEUNREAL_API  SCustomCombo: public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SCustomCombo){}
	SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	typedef TSharedPtr<FString> FComboItemType;

	FComboItemType CurrentItem;
	TArray<FComboItemType> Options;

	//Listeners
	FText GetCurrentItemLabel() const;
	
	void OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type)
	{
		CurrentItem = NewValue;
	}

	TSharedRef<SWidget> MakeWidgetForOption(FComboItemType InOption) const
	{
		return SNew(STextBlock).Text(FText::FromString(*InOption));
	}
};