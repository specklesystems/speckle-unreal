// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleManagerDetailsPanel.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"


TSharedRef<IDetailCustomization> SpeckleManagerDetailsPanel::MakeInstance()
{
	return MakeShareable(new SpeckleManagerDetailsPanel);
}

void SpeckleManagerDetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	// if multiple objects are being customized - fallback.
	if (Objects.Num() != 1)
	{
		return;
	}

	SpeckleManager = Cast<ASpeckleUnrealManager>(Objects[0].Get());
	
	//Edits a category. If it doesn't exist it creates a new one
    IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("Speckle Actions");

	const FText TitleText = FText::FromString("Speckle Tools");
	const FMargin ContentPadding = FMargin (5.f, 5.f);
	
	//Adding a custom row
	CustomCategory.AddCustomRow(FText::FromString("Outline Color Changing Category"))
	.ValueContent()
	.VAlign(VAlign_Center) // set vertical alignment to center
	.MaxDesiredWidth(600)
	[
		//Background Image
	    SNew(SOverlay)
	    + SOverlay::Slot()
	    .HAlign(HAlign_Fill)
	    .VAlign(VAlign_Fill)
	    .Padding(ContentPadding)
	    [
	        SNew(SImage)
	        .ColorAndOpacity(FColor::Black)
	    ]

        //Overlay Slot
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .Padding(ContentPadding)
        [
            // Title Text
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .Padding(ContentPadding)
            [
                SNew(STextBlock)
                .Text(TitleText)
                .Justification(ETextJustify::Center)
            ]
	        
            //Button Fetch
            + SVerticalBox::Slot()
            .Padding(ContentPadding)
            [
                SNew(SButton)
                .OnClicked(this, &SpeckleManagerDetailsPanel::FecthBtnListener)
                [ 
                    SNew(STextBlock)
                    .Text(FText::FromString("Fetch Commits"))
                    .Justification(ETextJustify::Center)
                ]
            ]

            //Button Import
            + SVerticalBox::Slot()
            .Padding(ContentPadding)
            [
                SNew(SButton)
                .OnClicked(this, &SpeckleManagerDetailsPanel::ImportObjBtnListener)
                [ 
                    SNew(STextBlock)
                    .Text(FText::FromString("Import Object"))
                    .Justification(ETextJustify::Center)
                ]
            ]

            //Dropdown
            + SVerticalBox::Slot()
            .Padding(ContentPadding)
            [
                SAssignNew(CustomComboBox, SCustomCombo)
            ]
        ]
	];
}

FReply SpeckleManagerDetailsPanel::FecthBtnListener()
{
	if(CustomComboBox.IsValid() && SpeckleManager != nullptr)
	{
		//CustomComboBox->Options.Add(MakeShareable(new FString("extra commit")));
		if(SpeckleManager != nullptr)
		{
			SpeckleManager->FetchCommits();
			auto commits = SpeckleManager->ArrayOfCommits;
			for (auto Commit : commits)
			{
				CustomComboBox->Options.Add(MakeShareable(new FString(Commit)));
			}
		}
	}
	return	FReply::Handled();
}

FReply SpeckleManagerDetailsPanel::ImportObjBtnListener()
{
	if(SpeckleManager!= nullptr)
	{
		auto SelectedLabel = CustomComboBox->GetCurrentItemLabel().ToString();		
		
		SpeckleManager->ImportSpeckleObject(SelectedLabel);
	}
	
	return	FReply::Handled();
}
