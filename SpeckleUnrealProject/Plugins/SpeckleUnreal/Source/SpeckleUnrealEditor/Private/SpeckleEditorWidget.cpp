// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleEditorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "ObjectEditorUtils.h"
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
			GEngine->AddOnScreenDebugMessage(1, 8.0f, FColor::Red,
				"[SPECKLE LOG]: No available speckle managers in the scene. Place and reopen the window");
		}
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
			CommitsCBox->ClearOptions();			
			//keep commits only from the selected branch
			auto CommitsList = SpeckleReceiver->FetchListOfCommits();
			for (auto C : CommitsList)
			{
				if(C.BranchName == SelectedBranch)
				{
					CommitsCBox->AddOption(C.Message + " [" + C.AuthorName + "]");
				}
			}
		}
	}
}

void USpeckleEditorWidget::FetchSpeckleBranches(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			BranchesCBox->ClearOptions();
			auto BranchesList = SpeckleReceiver->FetchListOfBranches();
			for(auto b : BranchesList)
			{
				BranchesCBox->AddOption(b.Name);
			}
		}
	}
}

void USpeckleEditorWidget::SpeckleManagerSelectionListener(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	const int32 Idx = SpeckleManagersCBox->FindOptionIndex(SelectedItem);
	CurrentSpeckleManager = SpeckleManagers[Idx];

	InitUI();
}

void USpeckleEditorWidget::BranchesBoxSelectionListener(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SelectedBranch = SelectedItem;
}

