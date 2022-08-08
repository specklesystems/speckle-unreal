
#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "PointCloud.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UPointCloud : public UBase
{
	GENERATED_BODY()

public:

	UPointCloud() : UBase(TEXT("Objects.Geometry.Pointcloud")) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FVector> Points;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FColor> Colors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<float> Sizes;

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
};
