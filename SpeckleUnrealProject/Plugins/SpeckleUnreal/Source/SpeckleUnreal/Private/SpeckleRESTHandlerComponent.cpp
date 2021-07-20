// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleRESTHandlerComponent.h"

// Sets default values for this component's properties
USpeckleRESTHandlerComponent::USpeckleRESTHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpeckleRESTHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	// cache speckleManager for runtime purposes
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
}


// Called every frame
void USpeckleRESTHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USpeckleRESTHandlerComponent::ImportSpeckleObject(int CurrIndex)
{

#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif

	if(SpeckleManager)
	{
		auto Commits = SpeckleManager->ArrayOfCommits;
		// check if within bounds and import
		if(CurrIndex <= Commits.Num() && CurrIndex >= 0)
		{
			const auto InputObjectRefID = Commits[CurrIndex].ReferenceObjectID;
			SpeckleManager->ObjectID = InputObjectRefID;
			SpeckleManager->ImportSpeckleObject();

			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SPECKLE LOG]: Speckle unreal commits array index out of bounds"));
	}
}

TArray<FSpeckleCommit> USpeckleRESTHandlerComponent::FetchListOfCommits()
{
	
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		SpeckleManager->FetchStreamItems(Commit);
		return SpeckleManager->ArrayOfCommits;
	}
	return {};
}

TArray<FSpeckleCommit> USpeckleRESTHandlerComponent::FetchListOfStreams()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("[SPECKLE: Unimplemented method]")));
	return {};
}

TArray<FSpeckleBranch> USpeckleRESTHandlerComponent::FetchListOfBranches()
{
	
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		SpeckleManager->FetchStreamItems(Branch);
		return SpeckleManager->ArrayOfBranches;
	}
	return {};
}

