
#pragma once

#include "CoreMinimal.h"
#include "Transport.h"

#include "MemoryTransport.generated.h"

/**
 * An in memory storage of speckle objects.
 */
UCLASS(Transient, BlueprintType)
class SPECKLEUNREAL_API UMemoryTransport : public UObject, public ITransport
{
	GENERATED_BODY()
	
	TMap<FString, TSharedPtr<FJsonObject>> SpeckleObjects;

public:
	
	virtual TSharedPtr<FJsonObject> GetSpeckleObject(const FString& ObjectId) const override;
	virtual void SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject) override;
	
	virtual bool HasObject(const FString& ObjectId) const override;
	
	virtual void CopyObjectAndChildren(const FString& ObjectId,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) override { unimplemented(); }
	
	
	UFUNCTION(BlueprintPure, Category = "Speckle|Transports")
	static UMemoryTransport* CreateEmptyMemoryTransport()
	{
		UMemoryTransport* Transport = NewObject<UMemoryTransport>();
		return Transport;
	}
};
