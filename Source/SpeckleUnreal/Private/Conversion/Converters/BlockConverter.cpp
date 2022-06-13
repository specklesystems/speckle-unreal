// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "Conversion/Converters/BlockConverter.h"

#include "Objects/Other/BlockInstance.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Engine/World.h"

UBlockConverter::UBlockConverter()
{
	SpeckleTypes.Add(UBlockInstance::StaticClass());
	
	BlockInstanceActorType = AActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}

UObject* UBlockConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World, TScriptInterface<ISpeckleConverter>&)
{
	const UBlockInstance* Block = Cast<UBlockInstance>(SpeckleBase);
	if(Block == nullptr) return nullptr;

	return BlockToNative(Block, World);
}

AActor* UBlockConverter::BlockToNative(const UBlockInstance* Block, UWorld* World)
{
	AActor* BlockActor = CreateEmptyActor(World, USpeckleObjectUtils::CreateTransform(Block->Transform));
	//Return the block actor as is,
	//Other converter logic will convert child geometries because UBlockInstance intentionally left them as dynamic properties
	return BlockActor;
}

AActor* UBlockConverter::CreateEmptyActor(UWorld* World, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AActor* Actor = World->SpawnActor<AActor>(BlockInstanceActorType, Transform, SpawnParameters);

	if(!Actor->HasValidRootComponent())
	{
		USceneComponent* Scene = NewObject<USceneComponent>(Actor, "Root");
		Scene->SetRelativeTransform(Transform);
		Actor->SetRootComponent(Scene);
		Scene->RegisterComponent();
	}
	USceneComponent* RootComponent = Actor->GetRootComponent();
	
	RootComponent->SetMobility(ActorMobility);
	
	return Actor;
}

UBase* UBlockConverter::ConvertToSpeckle_Implementation(const UObject* Object)
{
	unimplemented();
	return nullptr; //TODO implement
}
