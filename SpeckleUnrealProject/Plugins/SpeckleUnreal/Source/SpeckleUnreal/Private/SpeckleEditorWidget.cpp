// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleEditorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "SpeckleUnreal/Public/SpeckleUnrealManager.h"


void USpeckleEditorWidget::NativeConstruct()
{
	//bind widgets. To make it work in the editor we search based on the WidgetTree.
	StreamName = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("Stream")));
	CommitsCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("CommitsBox")));
	BranchesCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("BranchesBox")));
	SelectedCommitText = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("SelectedCommit")));
	SpeckleManagersCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("SpeckleManagersBox")));
	SpeckleManagersCBox->OnSelectionChanged.AddDynamic(this, &USpeckleEditorWidget::SpeckleManagerSelectionListener);

	//Grab all speckleManagers in the scene
	FindAllActors(GetWorld(), SpeckleManagers);

	if(SpeckleManagers.Num() > 0)
	{
		CurrentSpeckleManager = SpeckleManagers[0];

		//display speckle managers to combo box
		for(auto s : SpeckleManagers)
		{
			SpeckleManagersCBox->AddOption(s->GetName());
		}

		SpeckleManagersCBox->SetSelectedOption(CurrentSpeckleManager->GetName());
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red,
				"[Speckle]: No available speckle managers in the scene. Place and reopen the window");
		}
	}

	InitUI();
}

void USpeckleEditorWidget::InitUI()
{
	if(CurrentSpeckleManager)
	{
		StreamName->SetText(FText::FromString("Stream ID: " + CurrentSpeckleManager->StreamID));
		
		FetchButtonListener();
	}	
}

void USpeckleEditorWidget::ImportButtonListener()
{
	if(CurrentSpeckleManager != nullptr)
	{
		auto Commits = CurrentSpeckleManager->ArrayOfCommits;
		const auto CurrentIdx = CommitsCBox->FindOptionIndex(CommitsCBox->GetSelectedOption());
		if(CurrentIdx <= Commits.Num() && CurrentIdx >= 0)
		{
			const auto ObjectRefID = Commits[CurrentIdx].ReferenceObjectID;
			CurrentSpeckleManager->ObjectID = ObjectRefID;
			CurrentSpeckleManager->ImportSpeckleObject();
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SPECKLE LOGs]: Speckle unreal commits array index out of bounds"));
	}
}

void USpeckleEditorWidget::FetchButtonListener()
{	
	if(CurrentSpeckleManager != nullptr)
	{
		CurrentSpeckleManager->FetchCommits();
		CommitsCBox->ClearOptions();

		for(auto c : CurrentSpeckleManager->ArrayOfCommits)
		{
			CommitsCBox->AddOption(c.Message + " [" + c.AuthorName + "]");
		}
	}
}

void USpeckleEditorWidget::SpeckleManagerSelectionListener(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 Idx = SpeckleManagersCBox->FindOptionIndex(SelectedItem);
	CurrentSpeckleManager = SpeckleManagers[Idx];

	InitUI();
}

