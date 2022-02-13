#include "Conversion/Converters/BlockConverter.h"

#include "Objects/BlockInstance.h"

UBlockConverter::UBlockConverter()
{
	SpeckleTypes.Add(UBlockInstance::StaticClass());
	
	BlockInstanceActorType = AActor::StaticClass();
	ActorMobility = EComponentMobility::Static;
}

AActor* UBlockConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World)
{
	
	const UBlockInstance* Block = Cast<UBlockInstance>(SpeckleBase);
	if(Block == nullptr) return nullptr;

	return BlockToNative(Block);
}

AActor* UBlockConverter::BlockToNative(const UBlockInstance* Block)
{
	AActor* BlockActor = CreateEmptyActor(FTransform(Block->Transform));
	//Return the block actor as is,
	//Other converter logic will convert child geometries because UBlockInstance intentionally left them as dynamic properties
	return BlockActor;
}

AActor* UBlockConverter::CreateEmptyActor(const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
	AActor* Actor = GetWorld()->SpawnActor<AActor>(BlockInstanceActorType, Transform, SpawnParameters);
	USceneComponent* Scene = NewObject<USceneComponent>(Actor, "Root");
	Scene->SetRelativeTransform(Transform);
	Scene->SetMobility(ActorMobility);
	Actor->SetRootComponent(Scene);
	Scene->RegisterComponent();
	return Actor;
}

UBase* UBlockConverter::ConvertToSpeckle_Implementation(const UObject* Object)
{
	return nullptr; //TODO implement
}
