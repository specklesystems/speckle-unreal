// Copyright AEC Systems Ltd

#include "Objects/DynamicBase.h"
#include "Misc/Variant.h"

template <typename T>
bool UDynamicBase::TryGetDynamicProperty(const FString& Key, T& OutValue) const
{
	const auto Wrapper = DynamicProperties.FindRef(Key);
	if(Wrapper.IsValid()) return false;

	if(Wrapper->GetType() != TVariantTraits<T>::GetType()) return false;
	
	OutValue = Wrapper->GetValue<T>();
	return true;
}

template <typename T>
void UDynamicBase::SetDynamicProperty(const FString& Key, T Value)
{
	DynamicProperties.Add(Key, MakeShareable(new FVariant(Value)));
}

int32 UDynamicBase::RemoveDynamicProperty(const FString& Key)
{
	return DynamicProperties.Remove(Key);
}

//-------------------

bool UDynamicBase::TryGetDynamicStringProperty(const FString& Key, FString& OutValue) const
{
	return TryGetDynamicProperty(Key, OutValue);
}

bool UDynamicBase::TryGetDynamicIntProperty(const FString& Key, int32& OutValue) const
{
	return TryGetDynamicProperty(Key, OutValue);
}

bool UDynamicBase::TryGetDynamicFloatProperty(const FString& Key, float& OutValue) const
{
	return TryGetDynamicProperty(Key, OutValue);
}

bool UDynamicBase::TryGetDynamicBoolProperty(const FString& Key, bool& OutValue) const
{
	return TryGetDynamicProperty(Key, OutValue);
}

bool UDynamicBase::TryGetDynamicBaseProperty(const FString& Key, UBase*& OutValue) const
{
	return TryGetDynamicProperty(Key, OutValue);
}

void UDynamicBase::SetDynamicStringProperty(const FString& Key, FString& OutValue)
{
	SetDynamicProperty(Key, OutValue);
}

void UDynamicBase::SetDynamicIntProperty(const FString& Key, int32& OutValue)
{
	SetDynamicProperty(Key, OutValue);
}

void UDynamicBase::SetDynamicFloatProperty(const FString& Key, float& OutValue)
{
	SetDynamicProperty(Key, OutValue);
}

void UDynamicBase::SetDynamicBoolProperty(const FString& Key, bool& OutValue)
{
	SetDynamicProperty(Key, OutValue);
}

void UDynamicBase::SetDynamicBaseProperty(const FString& Key, UBase*& OutValue)
{
	SetDynamicProperty(Key, OutValue);
}

