#pragma once

// logs
#include "Engine/Engine.h"

// json manipulation
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// web requests
#include "Runtime/Online/HTTP/Public/Http.h"

#include "SpeckleUnrealMesh.h"
#include "SpeckleUnrealLayer.h"
#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

UCLASS(BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()

public:
	FHttpModule* Http;

	/* The actual HTTP call */
	UFUNCTION()
		void GetStream();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString ServerUrl {
		"https://hestia.speckle.works/api/"
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString StreamID {
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString AuthToken {
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		TSubclassOf<ASpeckleUnrealMesh> MeshActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		UMaterialInterface* DefaultMeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		bool RandomColorsPerLayer;

	TArray<USpeckleUnrealLayer*> SpeckleUnrealLayers;

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnStreamResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UWorld* World;

	float ScaleFactor;

	int32 LayerIndex;
	int32 CurrentObjectIndex;

	void SetUpGetRequest(TSharedRef<IHttpRequest> Request);

	void GetStreamObjects(int32 objectCount);

	void OnStreamObjectResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};