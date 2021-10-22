#pragma once

// json manipulation
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// web requests
#include "Runtime/Online/HTTP/Public/Http.h"

#include "SpeckleUnrealLayer.h"
#include "SpeckleUnrealMesh.h"
#include "GameFramework/Actor.h"
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
		FString ServerUrl {
		"https://speckle.xyz"
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString StreamID {
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString ObjectID {
		""
	};

	/** A Personal Access Token can be created from your Speckle Profile page (Treat tokens like passwords, do not share publicly) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString AuthToken {
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	bool ImportAtRuntime;
	

	/** You may specify a custom class inheriting from AASpeckleUnrealMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Mesh")
		TSubclassOf<ASpeckleUnrealMesh> MeshActor {
		ASpeckleUnrealMesh::StaticClass()
	};

	/** If true, simple collision will be calculated when creating static meshes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Mesh")
	bool BuildSimpleCollisions = false;

#if WITH_EDITOR
	/** If true, generated static meshes will be use the full editor-only build process, this has a dramatic effect on conversion times*/
	UPROPERTY(EditAnywhere, Category = "Speckle|Mesh")
	bool UseFullBuildProcess = false;
#endif
	

	
	/** Material to be applied to meshes when no RenderMaterial can be converted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Materials")
		UMaterialInterface* DefaultMeshMaterial;

	/** Material Parent for converted opaque materials*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Materials")
		UMaterialInterface* BaseMeshOpaqueMaterial;

	/** Material Parent for converted materials with an opacity less than one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Materials")
		UMaterialInterface* BaseMeshTransparentMaterial;
	
	/** When generating meshes, materials in this TMap will be used instead of converted ones if the key matches the ID of the Object's RenderMaterial. (Takes priority over name matching)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Materials|Overrides", DisplayName = "By Speckle ID")
	TMap<FString, UMaterialInterface*> MaterialOverridesById;

	/** When generating meshes, materials in this TSet will be used instead of converted ones if the material name matches the name of the Object's RenderMaterial. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Materials|Overrides", DisplayName = "By Name")
	TSet<UMaterialInterface*> MaterialOverridesByName;

	/** Materials converted from stream RenderMaterial objects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speckle|Materials")
	TMap<FString, UMaterialInterface*> ConvertedMaterials;

	TArray<USpeckleUnrealLayer*> SpeckleUnrealLayers;

	void OnStreamTextResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UWorld* World;
	
	float WorldToCentimeters;
	

	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;
	
	//TMap<FString, UObject*> CreatedObjectsCache;
	//TMap<FString, UObject*> InProgressObjectsCache;
	TArray<UObject*> CreatedObjectsCache;
	TArray<UObject*> InProgressObjectsCache;
	
	
	void ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject, const TSharedPtr<FJsonObject> ParentObject = nullptr);

	UMaterialInterface* CreateMaterial(TSharedPtr<FJsonObject> RenderMaterialObject, bool AcceptMaterialOverride = true);
	UMaterialInterface* CreateMaterial(const class URenderMaterial* SpeckleMaterial, bool AcceptMaterialOverride = true);
	ASpeckleUnrealMesh* CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent = nullptr);
	ASpeckleUnrealActor* CreateBlockInstance(const TSharedPtr<FJsonObject> Obj);
	
	TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField) const;
	float ParseScaleFactor(const FString& Units) const;
};