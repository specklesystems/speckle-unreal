#include "PanelButton.h"

void SPanelButton::Construct(const FArguments& InArgs)
{
	FSlateFontInfo TextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	TextStyle.Size = 10.0f;
	
	Label = InArgs._Label;
	OnButtonClicked = InArgs._ButtonClicked;
	ImageBrush = InArgs._ImageBrush;
	ChildSlot
	[
		SNew(SButton)
		.OnClicked(OnButtonClicked)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(0,0,5,0))
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text_Lambda([this] {return 
				FText::FromString(Label.Get());})
				.Font(TextStyle)
			]

			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
            .VAlign(EVerticalAlignment::VAlign_Center)
            .AutoWidth()
            [
				SNew(SImage)
				.Image_Lambda([this] {return ImageBrush.Get();})
            ]
		]
	];
}
