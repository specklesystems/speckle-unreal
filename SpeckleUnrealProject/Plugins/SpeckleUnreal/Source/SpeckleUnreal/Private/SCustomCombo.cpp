
#include "SCustomCombo.h"
#include "SCustomCombo.h"

void SCustomCombo::Construct(const FArguments& InArgs)
{
	Options.Add(MakeShareable(new FString("Commit 1")));
	Options.Add(MakeShareable(new FString("Commit 2")));
	Options.Add(MakeShareable(new FString("Commit 3")));
 
	CurrentItem = Options[0];
 
	ChildSlot
        [
			SNew(SComboBox<FComboItemType>)
            .OptionsSource(&Options)
            .OnSelectionChanged(this, &SCustomCombo::OnSelectionChanged)
            .OnGenerateWidget(this, &SCustomCombo::MakeWidgetForOption)
            .InitiallySelectedItem(CurrentItem)
            [
                SNew(STextBlock)
                .Text(this, &SCustomCombo::GetCurrentItemLabel)
            ]
        ];
}

FText SCustomCombo::GetCurrentItemLabel() const
{
	if (CurrentItem.IsValid())
	{
		return FText::FromString(*CurrentItem);
	}
 
	return FText::FromString("Invalid Combo box Item");
}