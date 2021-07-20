#pragma once

// logs
#include "Engine/Engine.h"

// json manipulation
#include "Dom/JsonObject.h"
// #include "Dom/JsonValue.h"
// #include "Serialization/JsonReader.h"
// #include "Serialization/JsonSerializer.h"

// web requests
#include "Runtime/Online/HTTP/Public/Http.h"

#include "SpeckleUnrealMesh.h"
#include "SpeckleUnrealLayer.h"
#include "GameFramework/Actor.h"
#include "SpeckleStructs.h"
#include "SpeckleUnrealManager.generated.h"


UCLASS(BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()

public:
	FHttpModule* Http;

	/* The actual HTTP call */
	UFUNCTION(CallInEditor, Category = "Speckle")
		void ImportSpeckleObject();

	UFUNCTION(CallInEditor, Category = "Speckle")
		void DeleteObjects();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ServerUrl
	{
		"https://speckle.xyz"
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString StreamID
	{
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString ObjectID
	{
		""
	};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString AuthToken
	{
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	TSubclassOf<ASpeckleUnrealMesh> MeshActor
	{
		ASpeckleUnrealMesh::StaticClass()
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	UMaterialInterface* DefaultMeshOpaqueMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	UMaterialInterface* DefaultMeshTransparentMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	bool ImportAtRuntime;

	TArray<USpeckleUnrealLayer*> SpeckleUnrealLayers;

	void OnStreamTextResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStreamCommitsListResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStreamBranchesListResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStreamItemsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<FSpeckleCommit> ArrayOfCommits;

	UPROPERTY()
	TArray<FSpeckleBranch> ArrayOfBranches;

	void FetchStreamItems(ESpeckleItemType ItemType);
	
protected:

	UWorld* World;

	float ScaleFactor;

	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;
	TMap<FString, TSharedPtr<FJsonObject>> SpeckleCommits;
	
	TMap<FString, ASpeckleUnrealMesh*> CreatedSpeckleMeshes;
	TMap<FString, ASpeckleUnrealMesh*> InProgressSpeckleMeshes;

	ASpeckleUnrealMesh* GetExistingMesh(const FString &objectId);

	void ImportObjectFromCache(const TSharedPtr<FJsonObject> speckleObject);

	UMaterialInterface* CreateMaterial(TSharedPtr<FJsonObject>);
	ASpeckleUnrealMesh* CreateMesh(TSharedPtr<FJsonObject>, UMaterialInterface *explicitMaterial = nullptr);
	
	TArray<uint8> FStringToUint8(const FString& InString)
	{
		TArray<uint8> OutBytes;

		// Handle empty strings
		if (InString.Len() > 0)
		{
			FTCHARToUTF8 Converted(*InString); // Convert to UTF8
			OutBytes.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
		}

		return OutBytes;
	}
};