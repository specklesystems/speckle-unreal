// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleEditorWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "SpeckleUnreal/Public/SpeckleUnrealManager.h"

USpeckleEditorWidget::USpeckleEditorWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USpeckleEditorWidget::NativeConstruct()
{
	StreamName = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("StreamN")));
	CommitsCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("CommitsBox")));
	BranchesCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("BranchesBox")));

	if(SpeckleUnrealManager == nullptr)
	{
		SpeckleUnrealManager = GetWorld()->SpawnActor<ASpeckleUnrealManager>(SpecklManager, FVector(0,0,0), FRotator::ZeroRotator);
	}
}

void USpeckleEditorWidget::ImportButtonListener()
{
	if(SpeckleUnrealManager != nullptr)
	{
		const auto SelectedObjectID = CommitsCBox->GetSelectedOption();
		SpeckleUnrealManager->ImportSpeckleObject(SelectedObjectID);
	}
}

void USpeckleEditorWidget::FetchButtonListener()
{	
	if(SpeckleUnrealManager != nullptr)
	{
		SpeckleUnrealManager->FetchCommits();
		CommitsCBox->ClearOptions();

		for(auto c : SpeckleUnrealManager->ArrayOfCommits)
		{
			CommitsCBox->AddOption(c);
		}
	}
}

