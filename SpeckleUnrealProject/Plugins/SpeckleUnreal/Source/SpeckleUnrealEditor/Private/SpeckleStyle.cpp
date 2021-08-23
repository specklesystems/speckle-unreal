#include "SpeckleStyle.h"
#include "Slate/SlateGameResources.h"
#include "FSpeckleUnrealEditorModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"


#define IMAGE_BRUSH(RelativePath,...) FSlateImageBrush(FSpeckleStyle::InContent(RelativePath,".png"), __VA_ARGS__)

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
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon48x48(48.0f, 48.0f);
	
	SpeckleStyleSet->Set("Speckle.MenuIcon", new IMAGE_BRUSH(TEXT("speckle_icon_64_64"), Icon40x40));

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


