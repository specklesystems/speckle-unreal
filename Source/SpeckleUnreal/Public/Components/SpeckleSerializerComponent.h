// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeckleSerializerComponent.generated.h"


class UBase;
UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API USpeckleSerializerComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	
public:

	
	// Sets default values for this component's properties
	USpeckleSerializerComponent();
	
	UBase* DeserializeBase(const TSharedPtr<FJsonObject> Object, const TMap<FString, TSharedPtr<FJsonObject>>& JsonStream);
};
