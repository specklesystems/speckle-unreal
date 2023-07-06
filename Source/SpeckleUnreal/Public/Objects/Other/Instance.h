
#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "Instance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SPECKLEUNREAL_API UInstance : public UBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<UBase*> Geometry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FMatrix Transform;
	
protected:
	UInstance(const FString& SpeckleType) : UBase(SpeckleType) {}

	UInstance() : UBase(TEXT("Objects.Other.BlockInstance")) {}
};	
