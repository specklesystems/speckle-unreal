
#include "FSpeckleUnrealEditorModule.h"

#include "EditorUtilityWidget.h"
#include "SpeckleUnrealManager.h"
#include "FSpeckleEditorCommands.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "SpecklePanel.h"
#include "Framework/Docking/LayoutExtender.h"

IMPLEMENT_MODULE(FSpeckleUnrealEditorModule, SpeckleUnrealEditor);

#define LOCTEXT_NAMESPACE "FSpeckleUnrealEditorModule"

static const FName SpeckleTabID("Speckle Tab");

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
	TSharedRef<class FGlobalTabmanager> tm = FGlobalTabmanager::Get();
	tm->UnregisterTabSpawner(SpeckleTabID);
	FSpeckleStyle::Shutdown();
}

TSharedRef<SDockTab> FSpeckleUnrealEditorModule::SpawnDockableTab(const FSpawnTabArgs& TabSpawnArgs)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab).TabRole(ETabRole::NomadTab)
	.ContentPadding(FMargin(10,10))
	[SNew(SpecklePanel)];

	//Start the editor Utility widget
	auto EditorUIClass = LoadClass<UEditorUtilityWidget>(nullptr,
	TEXT("EditorUtilityWidgetBlueprint'/SpeckleUnreal/EWBP_SpeckleEditorWindow.EWBP_SpeckleEditorWindow_C'"));
	
	UWorld* World = GEditor->GetEditorWorldContext().World();
	check(World);
	auto CreatedUMGWidget = Cast<UEditorUtilityWidget>(CreateWidget(World, EditorUIClass));

	//SpawnedTab->SetContent(CreatedUMGWidget->TakeWidget());
	
	return SpawnedTab;
}

void FSpeckleUnrealEditorModule::SpeckleButtonListener()
{
	//Register dockable tab
	FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(
            SpeckleTabID,
            FOnSpawnTab::CreateRaw(this, & FSpeckleUnrealEditorModule::SpawnDockableTab))
        .SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
        .SetDisplayName(FText::FromString(TEXT("Speckle Tab")));

		//Invoke tab
		TSharedRef<class FGlobalTabmanager> tm = FGlobalTabmanager::Get();
		tm->TryInvokeTab(SpeckleTabID);
}


void FSpeckleUnrealEditorModule::OnEditorWindowClosed(const TSharedRef<SWindow>&)
{
	UE_LOG(LogTemp, Warning, TEXT("[SPECKLE LOG] :Speckle editor window closed"));
	SpeckleEditorWindow = nullptr;
}

#undef LOCTEXT_NAMESPACE