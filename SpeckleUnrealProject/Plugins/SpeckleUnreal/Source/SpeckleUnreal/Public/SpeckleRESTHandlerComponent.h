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
	virtual void ImportSpeckleObject(int CurrIndex) override;
	virtual TArray<FSpeckleCommit> FetchListOfCommits() override;

	UPROPERTY()
	ASpeckleUnrealManager* SpeckleManager;
};
