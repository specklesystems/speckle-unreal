
#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "Collection.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UCollection : public UBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FString CollectionType;
	
	UCollection() : UBase(TEXT("Speckle.Core.Models.Collection")) {}
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
};
