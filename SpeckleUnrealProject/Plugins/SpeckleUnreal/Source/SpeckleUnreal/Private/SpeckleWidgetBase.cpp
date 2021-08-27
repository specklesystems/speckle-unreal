// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleWidgetBase.h"
#include "ISpeckleReceiver.h"


void USpeckleWidgetBase::ImportSpeckleObject(UActorComponent* SpeckleActorComponent, int CommitIndex)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			SpeckleReceiver->ImportSpeckleObject(CommitIndex);
		}
	}
}

void USpeckleWidgetBase::FetchSpeckleCommits(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			SpeckleReceiver->FetchListOfCommits();
		}
	}
}

void USpeckleWidgetBase::FetchSpeckleBranches(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			SpeckleReceiver->FetchListOfBranches();
		}
	}
}

void USpeckleWidgetBase::FetchSpeckleStreams(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			SpeckleReceiver->FetchListOfStreams();
		}
	}
}
