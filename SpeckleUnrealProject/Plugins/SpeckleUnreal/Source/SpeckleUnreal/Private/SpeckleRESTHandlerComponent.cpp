// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleRESTHandlerComponent.h"

// Sets default values for this component's properties
USpeckleRESTHandlerComponent::USpeckleRESTHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USpeckleRESTHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	// cache speckleManager for runtime purposes
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
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
		if(Commits.Num() > 0 && (CurrIndex <= Commits.Num() && CurrIndex >= 0))
		{
			const auto InputObjectRefID = Commits[CurrIndex].ReferenceObjectID;
			SpeckleManager->ObjectID = InputObjectRefID;
			SpeckleManager->ImportSpeckleObject();

			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[SPECKLE LOG]: Speckle unreal commits array index out of bounds"));
	}
}

void USpeckleRESTHandlerComponent::FetchListOfCommits(const FString& BranchName)
{
	
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		//FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + SpeckleManager->StreamID + "\\\"){id name commits {totalCount cursor items {id referencedObject authorName message branchName} } }}\"}";
		FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + SpeckleManager->StreamID + "\\\"){branch(name: \\\"" + BranchName + "\\\"){commits{items {id referencedObject authorName message branchName} } }}}\"}";
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse = [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{ SpeckleManager->OnCommitsItemsResponseReceived(Request, Response, bWasSuccessful); };

		SpeckleManager->FetchStreamItems(PostPayload, HandleResponse);
	}
}

void USpeckleRESTHandlerComponent::FetchListOfStreams()
{
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		FString PostPayload = "{\"query\": \"query{user {streams(limit:20) {totalCount items {id name description}}}}\"}";
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse = [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{ SpeckleManager->OnStreamItemsResponseReceived(Request, Response, bWasSuccessful); };
		
		SpeckleManager->FetchStreamItems(PostPayload, HandleResponse);
	}
}

void USpeckleRESTHandlerComponent::FetchListOfBranches()
{
	
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		FString PostPayload = "{\"query\": \"query{\\n stream (id: \\\"" + SpeckleManager->StreamID + "\\\"){\\n id\\n name\\n branches{\\n totalCount\\n cursor\\n items{\\n id\\n name\\n description\\n}\\n }\\n }\\n}\"}";
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse = [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{ SpeckleManager->OnBranchesItemsResponseReceived(Request, Response, bWasSuccessful); };
		
		SpeckleManager->FetchStreamItems(PostPayload, HandleResponse);
	}
}

void USpeckleRESTHandlerComponent::FetchGlobals()
{
#if WITH_EDITOR
	SpeckleManager = Cast<ASpeckleUnrealManager>(GetOwner());
#endif
	
	if(SpeckleManager)
	{
		
		FString PostPayload = "{\"query\": \"query{stream (id: \\\"" + SpeckleManager->StreamID + "\\\"){branch(name:  \\\"" + "globals" + "\\\"){commits{totalCount items{referencedObject}}}}}\"}";
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse = [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{ SpeckleManager->OnGlobalStreamItemsResponseReceived(Request, Response, bWasSuccessful); };
		
		SpeckleManager->FetchStreamItems(PostPayload, HandleResponse);
	}
}

