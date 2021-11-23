// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleEditorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Async/Async.h"
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
	BranchesCBox->OnSelectionChanged.AddDynamic(this,&USpeckleEditorWidget::BranchesBoxSelectionListener);

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
			GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Red,
				"[SPECKLE LOG]: No available speckle managers in the scene. Place and reopen the window");
		}
	}

	//Bind event handlers to fill in the received items.
	if(CurrentSpeckleManager != nullptr)
	{
		CurrentSpeckleManager->OnBranchesProcessedDynamic.AddDynamic(this, &USpeckleEditorWidget::SpeckleBranchesReceived);
		CurrentSpeckleManager->OnCommitsProcessedDynamic.AddDynamic(this, &USpeckleEditorWidget::SpeckleCommitsReceived);
	}
	
	InitUI();
}

void USpeckleEditorWidget::InitUI()
{
	if(CurrentSpeckleManager)
	{
		StreamName->SetText(FText::FromString("Stream ID: " + CurrentSpeckleManager->StreamID));		
	}	
}

void USpeckleEditorWidget::ImportSpeckleObject(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);	
		if(SpeckleReceiver)
		{
			const auto CurrentIdx = CommitsCBox->FindOptionIndex(CommitsCBox->GetSelectedOption());
			SpeckleReceiver->ImportSpeckleObject(CurrentIdx);
		}
	}
}

void USpeckleEditorWidget::FetchSpeckleCommits(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{			
			SpeckleReceiver->FetchListOfCommits(SelectedBranch);
		}
	}
}

void USpeckleEditorWidget::SpeckleCommitsReceived(const TArray<FSpeckleCommit>& CommitsList)
{
	CommitsCBox->ClearOptions();
	for (auto C : CommitsList)
	{
		CommitsCBox->AddOption(C.Message + " [" + C.AuthorName + "]");
	}
}

void USpeckleEditorWidget::FetchSpeckleBranches(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			SpeckleReceiver->FetchListOfBranches();
		}
	}
}

void USpeckleEditorWidget::SpeckleBranchesReceived(const TArray<FSpeckleBranch>& BranchesList)
{
	BranchesCBox->ClearOptions();
	for(auto b : BranchesList)
	{
		BranchesCBox->AddOption(b.Name);
	}

	if(BranchesCBox->GetOptionCount() > 0)
	{
		BranchesCBox->SetSelectedOption("main");
		SelectedBranch = TEXT("main");
	} 
}

void USpeckleEditorWidget::SpeckleManagerSelectionListener(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 Idx = SpeckleManagersCBox->FindOptionIndex(SelectedItem);
	CurrentSpeckleManager = SpeckleManagers[Idx];

	InitUI();
}

