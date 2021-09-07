
#include "FSpeckleUnrealEditorModule.h"

#include "EditorUtilityWidget.h"
#include "SpeckleUnrealManager.h"
#include "FSpeckleEditorCommands.h"

IMPLEMENT_MODULE(FSpeckleUnrealEditorModule, SpeckleUnrealEditor);

#define LOCTEXT_NAMESPACE "FSpeckleUnrealEditorModule"

void FSpeckleUnrealEditorModule::StartupModule()
{
	FSpeckleEditorCommands::Register();
	TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());
	FSpeckleStyle::Initialize();
	
	//map object to function
	CommandList->MapAction
	(
		FSpeckleEditorCommands::Get().SpeckleToolBarBtn,
		FExecuteAction::CreateRaw(this, &FSpeckleUnrealEditorModule::SpeckleButtonListener),
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
		&FSpeckleUnrealEditorModule::AddToolbarExtension)
	);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender); 
}

void FSpeckleUnrealEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ToolbarExtender->RemoveExtension(Extension.ToSharedRef());
	Extension.Reset();
	ToolbarExtender.Reset();
	FSpeckleStyle::Shutdown();
}

void FSpeckleUnrealEditorModule::SpeckleButtonListener()
{	
	if(SpeckleEditorWindow == nullptr)
	{	
		//Start the editor Utility widget
		auto EditorUIClass = LoadClass<UEditorUtilityWidget>(nullptr,
			TEXT("EditorUtilityWidgetBlueprint'/SpeckleUnreal/EWBP_SpeckleEditorWindow.EWBP_SpeckleEditorWindow_C'"));

		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));

		UWorld* World = GEditor->GetEditorWorldContext().World();
		check(World);
		auto CreatedUMGWidget = Cast<UEditorUtilityWidget>(CreateWidget(World, EditorUIClass));

		// create new slate window
		SpeckleEditorWindow = SNew(SWindow)
            .AutoCenter(EAutoCenter::None)
            .IsInitiallyMaximized(false)
            .ClientSize(FVector2D(300,500))
            .SizingRule(ESizingRule::UserSized)
            .SupportsMaximize(false)
            .SupportsMinimize(true)
            .CreateTitleBar(true)
            .HasCloseButton(true)
            .MaxHeight(600)
            .MaxWidth(400);

		// Use UMG in slate
		if(CreatedUMGWidget != nullptr)
		{
			SpeckleEditorWindow->SetContent(CreatedUMGWidget->TakeWidget());		
		}

		// Add Windows to slate app
		FSlateApplication & SlateApp = FSlateApplication::Get();
		if (MainFrameModule.GetParentWindow().IsValid())
		{
			SlateApp.AddWindow(SpeckleEditorWindow.ToSharedRef(), true);
		}

		//Bind delegate when window is closed
		OnSpeckleWindowClosed.BindRaw(this, &FSpeckleUnrealEditorModule::OnEditorWindowClosed);
		SpeckleEditorWindow->SetOnWindowClosed(OnSpeckleWindowClosed);
	}
	else // if editor window already exists
	{
		SpeckleEditorWindow->BringToFront();
	}
}

void FSpeckleUnrealEditorModule::OnEditorWindowClosed(const TSharedRef<SWindow>&)
{
	UE_LOG(LogTemp, Warning, TEXT("[SPECKLE LOG] :Speckle editor window closed"));
	SpeckleEditorWindow = nullptr;
}

#undef LOCTEXT_NAMESPACE