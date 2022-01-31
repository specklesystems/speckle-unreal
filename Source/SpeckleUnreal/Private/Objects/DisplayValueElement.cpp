// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/DisplayValueElement.h"

#include "SpeckleUnrealManager.h"
#include "Objects/Mesh.h"


TArray<FString> UDisplayValueElement::DisplayValueAliasStrings = {
	"displayValue",
	"@displayValue",
	"displayMesh"
	"@displayMesh"
};


bool UDisplayValueElement::AddDisplayValue(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	UMesh* DisplayMesh = Cast<UMesh>(Manager->DeserializeBase(Obj));
	const bool Valid = IsValid(DisplayMesh);
	if(Valid)
		this->DisplayValue.Add(DisplayMesh);
	return Valid;
}

bool UDisplayValueElement::Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	if(!Super::Parse(Obj, Manager)) return false;

	//Find display values
	for(const FString& Alias : DisplayValueAliasStrings)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Obj->TryGetObjectField(Alias, SubObjectPtr))
		{
			AddDisplayValue(*SubObjectPtr, Manager);
			DynamicProperties.Remove(Alias);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Obj->TryGetArrayField(Alias, SubArrayPtr))
		{
			for (const auto& ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (ArrayElement->TryGetObject(ArraySubObjPtr))
				{
					AddDisplayValue(*ArraySubObjPtr, Manager);
				}
			}
			DynamicProperties.Remove(Alias);
		}
	}

	return DisplayValue.Num() > 0;
}
