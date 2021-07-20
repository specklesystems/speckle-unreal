// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SpeckleUnrealManager.h"
#include "UObject/Interface.h"
#include "ISpeckleReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UISpeckleReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECKLEUNREAL_API IISpeckleReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ImportSpeckleObject(int CurrIndex) = 0;
	virtual TArray<FSpeckleCommit> FetchListOfCommits() = 0;
	virtual TArray<FSpeckleCommit> FetchListOfStreams() = 0;
	virtual TArray<FSpeckleBranch> FetchListOfBranches() = 0;
};
