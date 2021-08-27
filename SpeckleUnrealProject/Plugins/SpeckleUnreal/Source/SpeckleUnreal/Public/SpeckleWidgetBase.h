// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpeckleUnrealManager.h"
#include "Blueprint/UserWidget.h"
#include "SpeckleWidgetBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API USpeckleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable)
	void ImportSpeckleObject(UActorComponent* SpeckleReceiver, int CommitIndex);
    
    UFUNCTION(BlueprintCallable)
    void FetchSpeckleCommits(UActorComponent* SpeckleReceiver);

	UFUNCTION(BlueprintCallable)
    void FetchSpeckleBranches(UActorComponent* SpeckleReceiver);

	UFUNCTION(BlueprintCallable)
    void FetchSpeckleStreams(UActorComponent* SpeckleReceiver);
};
