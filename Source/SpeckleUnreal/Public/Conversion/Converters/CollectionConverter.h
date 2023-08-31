
#pragma once

#include "CoreMinimal.h"
#include "Conversion/SpeckleConverter.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"

#include "CollectionConverter.generated.h"

class UCollection;


/**
 * Converts Speckle Mesh objects into native actors with a procedural mesh component.
 * 
 * Compared with the StaticMeshConverter, this converter has some serious limitations
 * - Cannot convert displayValues,
 * - N-gon faces will be ignored,
 * - Meshes are transient, and won't persist on level reload
 */
UCLASS()
class SPECKLEUNREAL_API UCollectionConverter :  public UObject, public ISpeckleConverter
{
	GENERATED_BODY()

	CONVERTS_SPECKLE_TYPES()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TSubclassOf<AActor> ActorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ToNative")
	TEnumAsByte<EComponentMobility::Type> ActorMobility;
	
	// Sets default values for this actor's properties
	UCollectionConverter();
	
	virtual UObject* ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld* World,
													 TScriptInterface<ISpeckleConverter>& AvailableConverters) override;
	
	virtual UBase* ConvertToSpeckle_Implementation(const UObject* Object) override;
	
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual AActor* CollectionToNative(const UCollection* SpeckleCollection, UWorld* World);
	
	UFUNCTION(BlueprintCallable, Category="ToNative")
	virtual UCollection* CollectionToSpeckle(const AActor* Object);
	
	virtual AActor* CreateEmptyActor(UWorld* World, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
	
};
