// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

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

	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FString Name;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FVector Origin;

	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FVector UpDirection;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	FVector ForwardDirection;
	
	UPROPERTY(BlueprintReadWrite, Category="Speckle|Objects")
	bool IsOrthogonal;

	UView3D() : UBase(TEXT("Objects.BuiltElements.View")) {}
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;
};
