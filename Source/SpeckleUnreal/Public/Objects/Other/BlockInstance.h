
#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "BlockInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UBlockInstance : public UBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<UBase*> Geometry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FMatrix Transform;
	
	UBlockInstance() : UBase(TEXT("Objects.Other.BlockInstance")) {}

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;

};	
