// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpeckleUnrealActor.generated.h"

class UMeshDescriptionBase;

UCLASS(BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

	// Sets default values for this actor's properties
	ASpeckleUnrealActor();
	
};
