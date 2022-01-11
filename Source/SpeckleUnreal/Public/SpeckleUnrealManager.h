#pragma once

// json manipulation
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// web requests
#include "Runtime/Online/HTTP/Public/Http.h"

#include "SpeckleUnrealLayer.h"
#include "Conversion/SpeckleConverterComponent.h"
#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"


class URenderMaterial;

UCLASS(ClassGroup=(Speckle), BlueprintType)
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpeckleConverterComponent* Converter;
	
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
	

	void OnStreamTextResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ParseScaleFactor(const FString& Units) const;

	//TODO move to conversion functions
	UFUNCTION(BlueprintCallable)
	bool TryGetMaterial(const URenderMaterial* SpeckleMaterial, bool AcceptMaterialOverride,
	                    UMaterialInterface*& OutMaterial);


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasObject(const FString& Id) const;
	
	UBase* DeserializeBase(const TSharedPtr<FJsonObject> Obj) const;
	bool ResolveReference(const TSharedPtr<FJsonObject> Object, TSharedPtr<FJsonObject>& OutObject) const;
	
	TSharedPtr<FJsonObject, ESPMode::Fast> GetSpeckleObject(const FString& Id) const;
	
	
protected:

	
	UWorld* World;
	
	float WorldToCentimeters;
	

	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;

	UPROPERTY()
	TArray<UObject*> CreatedObjectsCache;
	TArray<UObject*> InProgressObjectsCache;
	
	
	void ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject);
	
	 //AActor* CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent = nullptr);
	 AActor* CreateBlockInstance(const TSharedPtr<FJsonObject> Obj);
	 //AActor* CreatePointCloud(const TSharedPtr<FJsonObject> Obj);
	
};