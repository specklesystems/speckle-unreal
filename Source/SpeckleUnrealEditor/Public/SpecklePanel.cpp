#include "SpecklePanel.h"

#include "EngineUtils.h"
#include "PanelButton.h"
#include "SpeckleUnreal/Public/Panagiotis/SpeckleRESTHandlerComponent.h"
#include "Engine/World.h"
#include "SpeckleStyle.h"
#include "Widgets/Layout/SScaleBox.h"

struct FSpeckleBranch;
struct FSpeckleCommit;

#define LOCTEXT_NAMESPACE "SpecklePanel"

//const auto TextStyle = FSpeckleStyle::Get()->GetFontStyle("Speckle.DefaultText");
FSlateFontInfo TextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");

void SpecklePanel::Construct(const FArguments& InArgs)
{
	TextStyle.Size = 12.0f;
	
	Init();
	if(CurrentSpeckleManager == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 9.0f, FColor::Red, FString::Printf(TEXT("No available Speckle managers, place on in the scene and reopen the window")));
		return;
	}
	
	// //set delegates
	CurrentSpeckleManager->OnBranchesProcessed.AddRaw(this, &SpecklePanel::SpeckleBranchesReceived);
	CurrentSpeckleManager->OnCommitsProcessed.AddRaw(this, &SpecklePanel::SpeckleCommitsReceived);
	
	FetchContent();

	for (auto SM : SpeckleManagers)
	{
		SpeckleManagerNames.Add(MakeShareable(new FString(SM->GetName())));
	}

	const auto StreamID = FString("Stream id: " + CurrentSpeckleManager->StreamID);		
	
	ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        .VAlign(VAlign_Top)
        .Padding(10)
        [
        	SNew(SOverlay)
            +SOverlay::Slot()
            .HAlign(EHorizontalAlignment::HAlign_Left)
            .VAlign(EVerticalAlignment::VAlign_Top)
            .Padding(10)
            [
				SNew(STextBlock)
				.Text(FText::FromString(StreamID))
				.Font(TextStyle)
            ]
            
            +SOverlay::Slot()
            .HAlign(EHorizontalAlignment::HAlign_Right)
            .VAlign(EVerticalAlignment::VAlign_Top)
            [
            	SNew(SBox)
            	.WidthOverride(32)
            	.HeightOverride(32)
            	[
            		SNew(SButton)
            		.HAlign(EHorizontalAlignment::HAlign_Fill)
            		.VAlign(EVerticalAlignment::VAlign_Fill)
					.OnClicked(this, &SpecklePanel::ReceiveButtonClicked)
					[
						SNew(SImage)
						.Image(FSpeckleStyle::Get()->GetBrush("Speckle.ChangeModeIcon"))
					]
            	]
            ]
		]

		+ SScrollBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
        .Padding(10)
        [
			SNew(SBox)
            .WidthOverride(140)
            .HeightOverride(40)
            [
        		SAssignNew(ManagersCBox, STextComboBox)
				.OptionsSource(&SpeckleManagerNames)
				.OnSelectionChanged(this, &SpecklePanel::OnSpeckleManagersDropdownChanged)
				.InitiallySelectedItem(SpeckleManagerNames[0])
        	]
        ]
		
		+ SScrollBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			HorizontalActionsPanel()
		]
    ];
}

SpecklePanel::~SpecklePanel()
{
	if(CurrentSpeckleManager == nullptr) return;
	CurrentSpeckleManager->OnBranchesProcessed.Clear();
	CurrentSpeckleManager->OnCommitsProcessed.Clear();
}

FReply SpecklePanel::ReceiveButtonClicked()
{
	SpeckleRestHandlerComp->ImportSpeckleObject(CommitsCBoxContent.Find(CommitsCBox->GetSelectedItem()));
	return FReply::Handled();
}

TSharedRef<SWidget> SpecklePanel::HorizontalActionsPanel()
{		
	auto HorizontalPanel = SNew(SOverlay)
	+SOverlay::Slot()
	.HAlign(EHorizontalAlignment::HAlign_Left)
	.VAlign(EVerticalAlignment::VAlign_Fill)
	.Padding(FMargin(10,10,50,10))
	[
		SNew(SBox)
        .WidthOverride(80)
        .HeightOverride(32)
        [
        	SAssignNew(BranchesCBox, STextComboBox)
			.OptionsSource(&BranchesCBoxContent)
			.OnSelectionChanged(this, &SpecklePanel::OnBranchesDropdownChanged)
			//.ComboBoxStyle(FSpeckleStyle::Get(), "ComboBox.BranchCBtnStyle")
        ]
	]

	+SOverlay::Slot()
    .HAlign(EHorizontalAlignment::HAlign_Left)
    .VAlign(EVerticalAlignment::VAlign_Fill)
	.Padding(FMargin(100,10,50,10))
    [
	    SNew(SBox)
	    .WidthOverride(80)
	    .HeightOverride(32)
	    [
			SAssignNew(CommitsCBox, STextComboBox)
			.OptionsSource(&CommitsCBoxContent)
	    ]
    ]
	
    +SOverlay::Slot()
    .HAlign(EHorizontalAlignment::HAlign_Right)
    .VAlign(EVerticalAlignment::VAlign_Fill)
	.Padding(FMargin(0,10,0,10))

    [
		SNew(SPanelButton)
		.Label(TEXT("Receive"))
		.ButtonClicked(this, &SpecklePanel::ReceiveButtonClicked)
		.ImageBrush(FSpeckleStyle::Get()->GetBrush("Speckle.ReceiveIcon"))
    ];

	return HorizontalPanel;
}

void SpecklePanel::OnSpeckleManagersDropdownChanged(TSharedPtr<FString> SelectedName, ESelectInfo::Type InSelectionInfo)
{
	// Early out if the new selection is the same as the old one
	check(SelectedName != nullptr);

	const int32 Idx = SpeckleManagerNames.Find(SelectedName);
	CurrentSpeckleManager = SpeckleManagers[Idx];
	SpeckleRestHandlerComp = CurrentSpeckleManager->FindComponentByClass<USpeckleRESTHandlerComponent>();
}

void SpecklePanel::OnBranchesDropdownChanged(TSharedPtr<FString> SelectedName, ESelectInfo::Type InSelectionInfo)
{
	SelectedBranch = SelectedName;
	SpeckleRestHandlerComp->FetchListOfCommits(*SelectedBranch.Get());
	//CommitsCBox->RefreshOptions();
}

void SpecklePanel::Init()
{
	const auto EditorWorld = GEditor->GetEditorWorldContext().World();
	if(EditorWorld)
	{
		//find all speckle managers in editor scene
		for(TActorIterator<ASpeckleUnrealManager> It(EditorWorld); It; ++It)
		{
			SpeckleManagers.Add(*It);
		}

		if(SpeckleManagers.Num() > 0)
		{
			//set speckleManager and component for REST calls
			CurrentSpeckleManager = SpeckleManagers[0];
			SpeckleRestHandlerComp = CurrentSpeckleManager->FindComponentByClass<USpeckleRESTHandlerComponent>();

			SelectedBranch = MakeShareable(new FString("main"));
		}
	}
	
}

void SpecklePanel::FetchContent() const
{
	check(SpeckleRestHandlerComp != nullptr)
	if(SpeckleRestHandlerComp == nullptr) return;

	SpeckleRestHandlerComp->FetchListOfBranches();
	
	check(SelectedBranch != nullptr)
	SpeckleRestHandlerComp->FetchListOfCommits(*SelectedBranch.Get());
}

void SpecklePanel::SpeckleBranchesReceived(const TArray<FSpeckleBranch>& BranchesList)
{
	BranchesCBoxContent.Empty();
	for(auto B : BranchesList)
	{
		BranchesCBoxContent.Add(MakeShareable(new FString(B.Name)));
	}
	BranchesCBox->RefreshOptions();
}

void SpecklePanel::SpeckleCommitsReceived(const TArray<FSpeckleCommit>& CommitsList)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), CommitsList.Num());
	CommitsCBoxContent.Empty();		
	for(auto C : CommitsList)
	{
		CommitsCBoxContent.Add(MakeShareable(new FString(C.Message + " [" + C.AuthorName + "]")));
	}

	CommitsCBox->RefreshOptions();
}

#undef LOCTEXT_NAMESPACE
