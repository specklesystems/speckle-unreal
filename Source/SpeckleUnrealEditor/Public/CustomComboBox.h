#pragma once

class SCustomComboBox: public SCompoundWidget
{
	
public:
	SLATE_BEGIN_ARGS(SCustomComboBox){}

	SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, ComboOptions)
	
	SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	SCustomComboBox();
	
	TSharedPtr<FString> CurrentItem;
	TArray<TSharedPtr<FString>> ComboOptions;

	//Listeners
	FText GetCurrentItemLabel() const;
	
	void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
	{
		CurrentItem = NewValue;
	}

	TSharedRef<SWidget> MakeWidgetForOption(TSharedPtr<FString> InOption) const
	{
		return SNew(STextBlock).Text(FText::FromString(*InOption));
	}
};
