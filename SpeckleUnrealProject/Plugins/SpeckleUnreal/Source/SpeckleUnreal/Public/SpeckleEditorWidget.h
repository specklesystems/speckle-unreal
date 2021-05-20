// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "SpeckleEditorWidget.generated.h"

class ASpeckleUnrealManager;

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API USpeckleEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	USpeckleEditorWidget(const FObjectInitializer& ObjectInitializer);
    
    void NativeConstruct() override;

    UPROPERTY()
    class UButton* ImportObjectButton;

    UPROPERTY()
    UButton* FetchButton;

    UPROPERTY()
    class UTextBlock* StreamName;

    UPROPERTY()
    class UComboBoxString* BranchesCBox;
    
    UPROPERTY()
    class UComboBoxString* CommitsCBox;

    UFUNCTION(BlueprintCallable)
    void ImportButtonListener();

    UFUNCTION(BlueprintCallable)
    void FetchButtonListener();

    UPROPERTY(EditAnywhere)
    TSubclassOf<ASpeckleUnrealManager> SpecklManagerClass;

	UPROPERTY()
	ASpeckleUnrealManager* SpeckleUnrealManager;
};
