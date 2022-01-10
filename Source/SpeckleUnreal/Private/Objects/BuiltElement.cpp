// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BuiltElement.h"

#include "SpeckleUnrealManager.h"


TArray<FString> UBuiltElement::DisplayValueAliasStrings = {
	"displayValue",
	"@displayValue",
	"displayMesh"
	"@displayMesh"
};


void UBuiltElement::AddDisplayValue(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	UBase* v = Manager->DeserializeBase(Obj);
	if(v != nullptr) this->DisplayValue.Add(v);
}

void UBuiltElement::Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	Super::Parse(Obj, Manager);

	//Find display values
	for(const FString& Alias : DisplayValueAliasStrings)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Obj->TryGetObjectField(Alias, SubObjectPtr))
		{
			AddDisplayValue(*SubObjectPtr, Manager);
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Obj->TryGetArrayField(Alias, SubArrayPtr))
		{
			for (const auto& ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (ArrayElement->TryGetObject(ArraySubObjPtr))
				{
					AddDisplayValue(*SubObjectPtr, Manager);
				}
			}
		}
	}
}
