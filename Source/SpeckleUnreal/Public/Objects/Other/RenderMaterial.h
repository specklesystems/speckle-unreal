
#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "RenderMaterial.generated.h"

/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API URenderMaterial : public UBase
{
	GENERATED_BODY()

public:
	
	URenderMaterial() : UBase(TEXT("Objects.Other.RenderMaterial")) {}

	UPROPERTY()
	FString Name;
	
	UPROPERTY()
	double Opacity = 1;
	
	UPROPERTY()
	double Metalness = 0;
	
	UPROPERTY()
	double Roughness = 1;
	
	UPROPERTY()
	FLinearColor Diffuse = FColor{221,221,221};
	
	UPROPERTY()
	FLinearColor Emissive = FLinearColor::Black;

	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override
	{
		if(!Super::Parse(Obj, ReadTransport)) return false;
	
		if(!Obj->TryGetStringField(TEXT("name"), Name)) return false;
		if(Obj->TryGetNumberField(TEXT("opacity"), Opacity)) DynamicProperties.Remove(TEXT("opacity"));
		if(Obj->TryGetNumberField(TEXT("metalness"), Metalness)) DynamicProperties.Remove(TEXT("metalness"));
		if(Obj->TryGetNumberField(TEXT("roughness"), Roughness)) DynamicProperties.Remove(TEXT("roughness"));

		bool IsValid = false;
		
		int32 ARGB;
		if(Obj->TryGetNumberField(TEXT("diffuse"), ARGB))
		{
			Diffuse = FColor(ARGB);
			DynamicProperties.Remove(TEXT("diffuse"));
			IsValid = true;
		}
		
		if(Obj->TryGetNumberField(TEXT("emissive"), ARGB))
		{
			Emissive = FColor(ARGB);
			DynamicProperties.Remove(TEXT("emissive"));
		}

		return IsValid;
	}

	
};
