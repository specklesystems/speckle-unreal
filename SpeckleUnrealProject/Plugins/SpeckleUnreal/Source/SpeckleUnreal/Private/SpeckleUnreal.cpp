// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpeckleUnreal.h"



#include "EditorUtilityWidget.h"
#include "SpeckleUnrealManager.h"
#include "SpeckleUnreal/Public/FSpeckleEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"

void FSpeckleUnrealModule::StartupModule()
{
	// Load custom details panel to speckleManager
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//Register the custom details panel we have created
	//PropertyModule.RegisterCustomClassLayout(ASpeckleUnrealManager::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&SpeckleManagerDetailsPanel::MakeInstance));

	FSpeckleEditorCommands::Register();
	TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());
	
	//map object to function
	CommandList->MapAction
	(
		FSpeckleEditorCommands::Get().SpeckleToolBarBtn,
		FExecuteAction::CreateRaw(this, &FSpeckleUnrealModule::SpeckleButtonListener),
		FCanExecuteAction()
	);

	//add to UE4 toolbar
	ToolbarExtender = MakeShareable(new FExtender());
	Extension = ToolbarExtender->AddToolBarExtension
	(
		"Misc",
		EExtensionHook::After,
		CommandList,
		FToolBarExtensionDelegate::CreateRaw(this,
		&FSpeckleUnrealModule::AddToolbarExtension)
	);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()
    ->AddExtender(ToolbarExtender); 
}

void FSpeckleUnrealModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ToolbarExtender->RemoveExtension(Extension.ToSharedRef());
	Extension.Reset();
	ToolbarExtender.Reset();
}

void FSpeckleUnrealModule::SpeckleButtonListener()
{
	//Start the editor Utility widget
	auto EditorUIClass = LoadClass<UEditorUtilityWidget>(nullptr, TEXT("EditorUtilityWidgetBlueprint'/SpeckleUnreal/EWBP_SpeckleWindow.EWBP_SpeckleWindow_C'"));

	UWorld* World = GEditor->GetEditorWorldContext().World();
	check(World);
	auto CreatedUMGWidget = Cast<UEditorUtilityWidget>(CreateWidget(World, EditorUIClass));

	FVector2D Size = (300, 500); // set a window size to your widget
	auto SpeckleWindow = SNew(SWindow).ClientSize(Size).MaxHeight(600).MaxWidth(400);

	if(CreatedUMGWidget != nullptr)
	{
		SpeckleWindow->SetContent(CreatedUMGWidget->TakeWidget());
		FSlateApplication::Get().AddWindow(SpeckleWindow, true);
	}
	else
	{
		
	}
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpeckleUnrealModule, SpeckleUnreal)