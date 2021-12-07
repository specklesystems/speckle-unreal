// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "NativeActors/Interfaces/SpeckleReceiver.h"
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
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void ImportSpeckleObject(int CurrIndex) override;

	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void ImportSpeckleObjectByRefObjId(FString RefObjId);
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void FetchListOfCommits(const FString& BranchName) override;

	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void FetchListOfStreams() override;
	
	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void FetchListOfBranches() override;

	UFUNCTION(BlueprintCallable, Category="Speckle REST")
	virtual void FetchGraphQL(const FString& CustomBearer, const FString& GraphQLPayload) override;
	
	UPROPERTY()
	ASpeckleUnrealManager* SpeckleManager;
};
