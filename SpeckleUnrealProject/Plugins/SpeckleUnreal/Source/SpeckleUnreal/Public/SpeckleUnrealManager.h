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
#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

UCLASS()
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()

public:
	FHttpModule* Http;

	/* The actual HTTP call */
	UFUNCTION()
		void GetStream();

	UPROPERTY(EditAnywhere, Category = "Speckle")
		FString ServerUrl {
		"https://hestia.speckle.works/api/"
	};

	UPROPERTY(EditAnywhere, Category = "Speckle")
		FString StreamID {
		""
	};

	UPROPERTY(EditAnywhere, Category = "Speckle")
		FString AuthToken {
		""
	};

	UPROPERTY(EditAnywhere, Category = "Speckle")
		TSubclassOf<ASpeckleUnrealMesh> MeshActor;

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnStreamResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UWorld* World;

	float ScaleFactor;

	void SetUpGetRequest(TSharedRef<IHttpRequest> Request);

	void GetStreamObjects(int32 objectCount);

	void OnStreamObjectResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};