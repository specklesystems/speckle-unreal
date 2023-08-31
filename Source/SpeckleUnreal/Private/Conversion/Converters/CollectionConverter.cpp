
#include "Conversion/Converters/CollectionConverter.h"

#include "Objects/Collection.h"

UCollectionConverter::UCollectionConverter()
{
    SpeckleTypes.Add(UCollection::StaticClass());

    
    ActorType = AActor::StaticClass();
    ActorMobility = EComponentMobility::Static;
}

UObject* UCollectionConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World,
                                                            TScriptInterface<ISpeckleConverter>& AvailableConverters )
{
    const UCollection* p = Cast<UCollection>(SpeckleBase);
	
    if(p == nullptr) return nullptr;
	
    return CollectionToNative(p, World);
}

AActor* UCollectionConverter::CollectionToNative(const UCollection*, UWorld* World)
{
    AActor* MeshActor = CreateEmptyActor(World);

    
    return MeshActor;
}

AActor* UCollectionConverter::CreateEmptyActor(UWorld* World, const FActorSpawnParameters& SpawnParameters)
{
    AActor* Actor = World->SpawnActor<AActor>(ActorType, SpawnParameters);
    USceneComponent* Scene = NewObject<USceneComponent>(Actor, "Root");
    Actor->SetRootComponent(Scene);
    Scene->RegisterComponent();
    Scene->SetMobility(ActorMobility);
    return Actor;
}


UBase* UCollectionConverter::ConvertToSpeckle_Implementation(const UObject* Object)
{
    return nullptr; //TODO implement ToSpeckle function
}


UCollection* UCollectionConverter::CollectionToSpeckle(const AActor* Object)
{
    return nullptr; //TODO implement ToSpeckle function
}
