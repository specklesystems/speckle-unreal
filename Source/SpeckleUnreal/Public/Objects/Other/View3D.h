// Copyright AEC Systems Ltd

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"
#include "View3D.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UView3D : public UBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString Name;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Origin;

	UPROPERTY(BlueprintReadWrite)
	FVector UpDirection;
	
	UPROPERTY(BlueprintReadWrite)
	FVector ForwardDirection;
	
	UPROPERTY(BlueprintReadWrite)
	bool IsOrthogonal;

	UView3D() : UBase(TEXT("Objects.BuiltElements.View")) {}
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
};
