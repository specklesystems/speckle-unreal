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

TArray<FSpeckleCommit> USpeckleWidgetBase::FetchSpeckleCommits(UActorComponent* SpeckleActorComponent)
{
	if(SpeckleActorComponent != nullptr)
	{
		const auto SpeckleReceiver = Cast<IISpeckleReceiver>(SpeckleActorComponent);
		if(SpeckleReceiver)
		{
			return SpeckleReceiver->FetchListOfCommits();
		}
	}
	return {};
}
