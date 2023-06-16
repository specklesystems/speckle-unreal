
#pragma once

#include "CoreMinimal.h"
#include "Instance.h"

#include "RevitInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API URevitInstance : public UInstance
{
	GENERATED_BODY()
	
public:
	
	URevitInstance() : UInstance(TEXT("Objects.Other.Revit.RevitInstance")) {}

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;

};
