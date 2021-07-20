// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ISpeckleReceiver.h"
#include "Components/ActorComponent.h"
#include "SpeckleRESTHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPECKLEUNREAL_API USpeckleRESTHandlerComponent : public UActorComponent, public IISpeckleReceiver
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpeckleRESTHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void ImportSpeckleObject(int CurrIndex) override;
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual TArray<FSpeckleCommit> FetchListOfCommits() override;

	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual TArray<FSpeckleCommit> FetchListOfStreams() override;
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual TArray<FSpeckleBranch> FetchListOfBranches() override;
	
	UPROPERTY()
	ASpeckleUnrealManager* SpeckleManager;
};
