#pragma once

// json manipulation
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

// web requests
#include "Runtime/Online/HTTP/Public/Http.h"

#include "Conversion/SpeckleConverterComponent.h"
#include "GameFramework/Actor.h"
#include "SpeckleUnrealManager.generated.h"

class UMemoryTransport;
class URenderMaterial;

UCLASS(ClassGroup=(Speckle), BlueprintType)
class SPECKLEUNREAL_API ASpeckleUnrealManager : public AActor
{
	GENERATED_BODY()
	
private:
	TScriptInterface<ITransport> MemoryTransport;
	
public:
	
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
	
	// Sets default values for this actor's properties
	ASpeckleUnrealManager();

	UFUNCTION(CallInEditor)
	void ReceiveCPP();

	UFUNCTION(CallInEditor)
	void DeleteObjects();
protected:
	
	UPROPERTY()
	TArray<AActor*> CreatedObjectsCache;
	
	void HandleReceive(TSharedPtr<FJsonObject> RootObject);
	
	void HandleError(FString& Message);
};