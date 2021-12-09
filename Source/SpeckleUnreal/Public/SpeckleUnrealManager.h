#pragma once

// logs
#include "Engine/Engine.h"

// json manipulation
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// web requests
#include "SpeckleUnrealActor.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include "SpeckleUnrealLayer.h"
#include "GameFramework/Actor.h"
#include "Objects/MetaInformation.h"
#include "NativeActors/SpeckleUnrealPointCloud.h"
#include "NativeActors/SpeckleUnrealProceduralMesh.h"
#include "SpeckleUnrealManager.generated.h"


class URenderMaterial;

UCLASS(BlueprintType, Blueprintable)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()

	// Asynchronously fetch of Streams, Branches, Commits, (access in Blueprints)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBranchesRequestProcessedDyn, const TArray<FSpeckleBranch>&, BranchesList);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommitsRequestProcessedDyn, const TArray<FSpeckleCommit>&, CommitsList);	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamsRequestProcessedDyn, const TArray<FSpeckleStream>&, StreamsList);	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGlobalsRequestProcessedDyn,
													const FSpeckleGlobals&, GlobalsObject, const FString&, StreamID);	
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommitJsonParsedDyn, const FString&, Completed);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJsonGraphQLProcessedDyn, const FString&, Completed);

	

	DECLARE_MULTICAST_DELEGATE_OneParam(FBranchesRequestProcessed, const TArray<FSpeckleBranch>&);

	DECLARE_MULTICAST_DELEGATE_OneParam(FCommitsRequestProcessed, const TArray<FSpeckleCommit>&);
	
public:
	FHttpModule* Http;

	/* The actual HTTP call */
	UFUNCTION(CallInEditor, Category = "Speckle")
		void ImportSpeckleObject();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Speckle")
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

	/** A Personal Access Token can be created from your Speckle Profile page (Treat tokens like passwords, do not share publicly) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
		FString AuthToken {
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	bool ImportAtRuntime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString Longitude
	{
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString Latitude
	{
		""
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	FString Height
	{
		""
	};
	
	
	/** The type of Actor to use for Mesh conversion, you may create a custom actor implementing ISpeckleMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Conversion", meta = (MustImplement = "SpeckleMesh"))
		TSubclassOf<ASpeckleUnrealActor> MeshActor {
		ASpeckleUnrealProceduralMesh::StaticClass()
	};
	
	/** The type of Actor to use for PointCloud conversion, you may create a custom actor implementing ISpecklePointCloud */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle|Conversion", meta = (MustImplement = "SpecklePointCloud"))
	TSubclassOf<AActor> PointCloudActor {
		ASpeckleUnrealPointCloud::StaticClass()
	};
	
	
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
	void OnCommitsItemsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnBranchesItemsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStreamItemsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGraphQLJsonReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	

	//void OnGlobalStreamItemsResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnCommitJsonParsedFinished(AActor* AOwner,
								const TSharedPtr<FJsonObject> SpeckleJsonObject,
								const TSharedPtr<FJsonObject> ParentJsonObject,
								TMap<FString, FString> ObjectsMapIn,
								FString Who);

	
	TMap<FString, FString> ImportObjectFromCacheNew(
								AActor* AOwner,
								const TSharedPtr<FJsonObject> SpeckleJsonObject,
								const TSharedPtr<FJsonObject> ParentJsonObject,
								TMap<FString, FString> ObjectsMapIn,
								FString who);
	

	
	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<FSpeckleCommit> ArrayOfCommits;

	UPROPERTY()
	TArray<FSpeckleBranch> ArrayOfBranches;

	UPROPERTY()
	TArray<FSpeckleStream> ArrayOfStreams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speckle")
	TMap<FString, FString> ObjectsMap;
	
	void FetchStreamItems(FString PostPayload, TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse);
	
	void FetchJson(const FString& CustomBearer, const FString& GraphQLPayload,
			TFunction<void(FHttpRequestPtr, FHttpResponsePtr , bool)> HandleResponse);

	
	void FetchGlobalItems(FString PostPayload, const FString& RefObjectID);

	

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FBranchesRequestProcessedDyn OnBranchesProcessedDynamic;

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FCommitsRequestProcessedDyn OnCommitsProcessedDynamic;

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FStreamsRequestProcessedDyn OnStreamsProcessedDynamic;

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FCommitJsonParsedDyn OnCommitJsonParsedDynamic;

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FGlobalsRequestProcessedDyn OnGlobalsProcessedDynamic;

	UPROPERTY(BlueprintAssignable, Category = "SpeckleEvents");
	FJsonGraphQLProcessedDyn OnGraphQLProcessedDynamic;
	

	FBranchesRequestProcessed OnBranchesProcessed;
	FCommitsRequestProcessed OnCommitsProcessed;

	UFUNCTION(BlueprintCallable)
	void FetchGlobalVariables(const FString& ServerName, const FString& Stream, const FString& Bearer);

	
	


	
	TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField) const;

	float ParseScaleFactor(const FString& Units) const;
	
	bool TryGetMaterial(const URenderMaterial* SpeckleMaterial, bool AcceptMaterialOverride,
	                    UMaterialInterface*& OutMaterial);

	// Dimitrios: Under construction: Import objects, Return a Map of Meshes to Layers correspondence


	
	
	
	
	
protected:

	UWorld* World;
	
	float WorldToCentimeters;
	

	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;
	TMap<FString, TSharedPtr<FJsonObject>> SpeckleCommits;
	
	TArray<UObject*> CreatedObjectsCache;
	TArray<UObject*> InProgressObjectsCache;

	
	void ImportObjectFromCache(AActor* AOwner,
							   const TSharedPtr<FJsonObject> SpeckleJsonObject,
							   const TSharedPtr<FJsonObject> ParentJsonObject = nullptr);




	
	ASpeckleUnrealActor* CreateMesh(const TSharedPtr<FJsonObject> Obj, const TSharedPtr<FJsonObject> Parent = nullptr);
	ASpeckleUnrealActor* CreateBlockInstance(const TSharedPtr<FJsonObject> Obj);
	AActor* CreatePointCloud(const TSharedPtr<FJsonObject> Obj);


	
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