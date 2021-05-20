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
	StreamName = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("Stream")));
	CommitsCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("CommitsBox")));
	BranchesCBox = Cast<UComboBoxString>(WidgetTree->FindWidget(TEXT("BranchesBox")));

	if(SpeckleUnrealManager == nullptr)
	{
		SpeckleUnrealManager = GetWorld()->SpawnActor<ASpeckleUnrealManager>(SpecklManagerClass, FVector(0,0,0), FRotator::ZeroRotator);
	}

	if(SpeckleUnrealManager != nullptr)
	{
		StreamName->SetText(FText::FromString("Stream ID: " + SpeckleUnrealManager->StreamID));
		FetchButtonListener();
	}
}

void USpeckleEditorWidget::ImportButtonListener()
{
	if(SpeckleUnrealManager != nullptr)
	{
		auto Commits = SpeckleUnrealManager->ArrayOfCommits;
		const auto CurrentIdx = CommitsCBox->FindOptionIndex(CommitsCBox->GetSelectedOption());
		if(CurrentIdx <= Commits.Num() && CurrentIdx > 0)
		{
			const auto ObjectRefID = Commits[CurrentIdx].ReferenceObjectID;
			UE_LOG(LogTemp, Warning, TEXT("%s"), *ObjectRefID);
			SpeckleUnrealManager->ImportSpeckleObject(ObjectRefID);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Speckle unreal commits array index out of bounds"));
		
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
			CommitsCBox->AddOption(c.Message + "[" + c.AuthorName + "]");
		}
	}
}

