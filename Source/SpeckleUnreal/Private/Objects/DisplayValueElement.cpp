
#include "Objects/DisplayValueElement.h"

#include "API/SpeckleSerializer.h"
#include "Objects/Geometry/Mesh.h"


TArray<FString> UDisplayValueElement::DisplayValueAliasStrings = {
	"displayValue",
	"@displayValue",
};


bool UDisplayValueElement::AddDisplayValue(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	UMesh* DisplayMesh = Cast<UMesh>(USpeckleSerializer::DeserializeBase(Obj, ReadTransport));
	const bool Valid = IsValid(DisplayMesh);
	if(Valid)
		this->DisplayValue.Add(DisplayMesh);
	return Valid;
}

bool UDisplayValueElement::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;

	//Find display values
	for(const FString& Alias : DisplayValueAliasStrings)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Obj->TryGetObjectField(Alias, SubObjectPtr))
		{
			AddDisplayValue(*SubObjectPtr, ReadTransport);
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
					AddDisplayValue(*ArraySubObjPtr, ReadTransport);
				}
			}
			DynamicProperties.Remove(Alias);
		}
	}

	return DisplayValue.Num() > 0;
}
