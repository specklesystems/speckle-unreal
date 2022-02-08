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
	
	void OnStreamTextResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//BEGIN REGION - This 
	
	TArray<TSharedPtr<FJsonValue>> CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ParseScaleFactor(const FString& Units) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasObject(const FString& Id) const;
	
	UBase* DeserializeBase(const TSharedPtr<FJsonObject> Obj) const;
	bool ResolveReference(const TSharedPtr<FJsonObject> Object, TSharedPtr<FJsonObject>& OutObject) const;
	
	TSharedPtr<FJsonObject, ESPMode::Fast> GetSpeckleObject(const FString& Id) const;
	
	
protected:

	
	UWorld* World;
	
	float WorldToCentimeters;

	bool TryParseSpeckleObjectFromJsonProperty(const TSharedPtr<FJsonValue> JsonValue, UBase*& OutBase) const;

	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;

	UPROPERTY()
	TArray<UObject*> CreatedObjectsCache;
	TArray<UObject*> InProgressObjectsCache;
	
	void ImportObjectFromCache(AActor* AOwner, const TSharedPtr<FJsonObject> SpeckleObject);
	
	
};