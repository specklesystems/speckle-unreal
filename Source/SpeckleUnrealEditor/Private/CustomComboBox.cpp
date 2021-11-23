#include "CustomComboBox.h"

void SCustomComboBox::Construct(const FArguments& InArgs)
{
	ComboOptions = InArgs._ComboOptions;
	ComboOptions.Num() > 0 ? CurrentItem = ComboOptions[0] : CurrentItem = MakeShareable(new FString("No available Speckle Managers"));
 
	ChildSlot
    [
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ComboOptions)
		.OnSelectionChanged(this, &SCustomComboBox::OnSelectionChanged)
		.OnGenerateWidget(this, &SCustomComboBox::MakeWidgetForOption)
		.InitiallySelectedItem(CurrentItem)
		[
			SNew(STextBlock)
			.Text(this, &SCustomComboBox::GetCurrentItemLabel)
		]
    ];
}

SCustomComboBox::SCustomComboBox()
{
#if WITH_ACCESSIBILITY
	AccessibleBehavior = EAccessibleBehavior::Auto;
	bCanChildrenBeAccessible = true;
#endif
}

FText SCustomComboBox::GetCurrentItemLabel() const
{
	if (CurrentItem.IsValid())
	{
		return FText::FromString(*CurrentItem);
	}
 
	return FText::FromString("Invalid Combo box Item");
}
