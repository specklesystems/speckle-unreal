// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeckleUnrealActor.h"

// Sets default values
ASpeckleUnrealActor::ASpeckleUnrealActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Scene;
	Scene->SetMobility(EComponentMobility::Static);
}


