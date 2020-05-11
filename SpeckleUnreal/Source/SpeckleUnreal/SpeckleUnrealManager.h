#pragma once

#include "SpeckleUnrealMesh.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
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
		"https://hestia.speckle.works/"
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

	void SetUpGetRequest(TSharedRef<IHttpRequest> Request);

	void GetStreamObjects(int32 objectCount);

	void OnStreamObjectResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};