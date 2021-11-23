#include "SpeckleStyle.h"
#include "Slate/SlateGameResources.h"
#include "FSpeckleUnrealEditorModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"


#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(FSpeckleStyle::InContent(RelativePath,".png"), __VA_ARGS__)
#define BOX_BRUSH(RelativePath,...) FSlateBoxBrush(FSpeckleStyle::InContent(RelativePath,".png"), __VA_ARGS__)

FString FSpeckleStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("SpeckleUnreal"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr<FSlateStyleSet>FSpeckleStyle::SpeckleStyleSet = nullptr;

TSharedPtr<ISlateStyle> FSpeckleStyle::Get()
{
	return SpeckleStyleSet;
}

void FSpeckleStyle::Initialize()
{
	// Only register once
	if (SpeckleStyleSet.IsValid())
	{
		return;
	}

	SpeckleStyleSet = MakeShareable<FSlateStyleSet>(new FSlateStyleSet(GetStyleSetName()));

	// Const icon sizes
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon32x32(32.0f, 32.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon48x48(48.0f, 48.0f);

	//set menu/toolbar icon
	SpeckleStyleSet->Set("Speckle.MenuIcon", new IMAGE_BRUSH(TEXT("speckle_icon_64_64"), Icon40x40));

	//set UI panel icons and buttons
	SpeckleStyleSet->Set("Speckle.BranchIcon", new IMAGE_BRUSH(TEXT("branch_64-64"), Icon24x24));
	SpeckleStyleSet->Set("Speckle.CommitsIcon", new IMAGE_BRUSH(TEXT("branch_64-64"), Icon24x24));
	SpeckleStyleSet->Set("Speckle.ChangeModeIcon", new IMAGE_BRUSH(TEXT("outline_sync_alt_black_24dp"), Icon32x32));
	SpeckleStyleSet->Set("Speckle.ReceiveIcon", new IMAGE_BRUSH(TEXT("receiver"), Icon24x24));

	const FButtonStyle BranchBtnStyle = FButtonStyle()
	.SetNormal( BOX_BRUSH("outline_sync_alt_black_24dp", FVector2D(32,32 )));
	// .SetHovered( BOX_BRUSH("outline_sync_alt_black_24dp", FVector2D(32,32), 8.0f/32.0f ) )
	// .SetPressed( BOX_BRUSH("outline_sync_alt_black_24dp", FVector2D(32,32), 8.0f/32.0f ) );
	SpeckleStyleSet->Set( "Buttons.BranchButtonStyle", BranchBtnStyle);

	//Combo button styles
	const FComboButtonStyle BranchesComboBtnStyle = FComboButtonStyle()
    .SetDownArrowImage(BOX_BRUSH(TEXT("branch_64-64"), Icon24x24));
	SpeckleStyleSet->Set("ComboBox.BranchCBtnStyle", BranchesComboBtnStyle);

	//Set text style
	FSlateFontInfo TextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	TextStyle.Size = 10.0F;
	SpeckleStyleSet->Set("Speckle.DefaultText", TextStyle);

	FSlateStyleRegistry::RegisterSlateStyle(*SpeckleStyleSet.Get());
}

void FSpeckleStyle::Shutdown()
{
	if (SpeckleStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*SpeckleStyleSet.Get());
		ensure(SpeckleStyleSet.IsUnique());
		SpeckleStyleSet.Reset();
	}
}

FName FSpeckleStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SpeckleStyle"));
	return StyleSetName;
}

#undef IMAGE_BRUSH


