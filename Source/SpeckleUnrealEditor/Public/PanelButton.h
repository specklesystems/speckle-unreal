#pragma once

class SPanelButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPanelButton)
	: _Label(TEXT("Default Value"))
	, _ImageBrush()
    , _ButtonClicked()
	{}

	SLATE_ATTRIBUTE(FString, Label)
	SLATE_ATTRIBUTE(const FSlateBrush*, ImageBrush)
	SLATE_EVENT(FOnClicked, ButtonClicked)
	
    SLATE_END_ARGS()

	void Construct(const FArguments&InArgs);
	TAttribute<FString> Label;
	TAttribute<const FSlateBrush*> ImageBrush;
	FOnClicked OnButtonClicked;
};
