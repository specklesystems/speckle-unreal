
#pragma once

#include "CoreMinimal.h"
#include "Instance.h"

#include "BlockInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UBlockInstance : public UInstance
{
	GENERATED_BODY()
	
public:
	
	UBlockInstance() : UInstance(TEXT("Objects.Other.BlockInstance")) {}

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;

};	
