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

    UPROPERTY(EditAnywhere)
    class UButton* ImportObjectButton;

    UPROPERTY(EditAnywhere)
    UButton* FetchButton;

    UPROPERTY(EditAnywhere)
    class UTextBlock* StreamName;

    UPROPERTY(EditAnywhere)
    class UComboBoxString* BranchesCBox;
    
    UPROPERTY(EditAnywhere)
    class UComboBoxString* CommitsCBox;

    UFUNCTION(BlueprintCallable)
    void ImportButtonListener();

    UFUNCTION(BlueprintCallable)
    void FetchButtonListener();

    UPROPERTY(EditAnywhere)
    TSubclassOf<ASpeckleUnrealManager> SpecklManager;

	ASpeckleUnrealManager* SpeckleUnrealManager;
};
