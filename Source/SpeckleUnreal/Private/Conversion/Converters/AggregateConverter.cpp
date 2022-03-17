// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/AggregateConverter.h"

#include "LogSpeckle.h"



void UAggregateConverter::OnConvertersChangeHandler()
{
	SpeckleTypeMap.Empty();

	for(int i = 0; i < SpeckleConverters.Num(); i++)
	{
		const UObject* Converter = SpeckleConverters[i];
		if(Converter != nullptr && !Converter->GetClass()->ImplementsInterface(USpeckleConverter::StaticClass()))
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface %s"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
			SpeckleConverters.RemoveAt(i);
			i--;
		}
	}
}

#if WITH_EDITOR
void UAggregateConverter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAggregateConverter, SpeckleConverters))
	{
		OnConvertersChangeHandler();
	}
}
#endif

UBase* UAggregateConverter::ConvertToSpeckle_Implementation(const UObject* Object)
{
	//TODO implement ToSpeckle
	unimplemented();
	return nullptr;
}


UObject* UAggregateConverter::ConvertToNative_Implementation(const UBase* Object, UWorld* World,  TScriptInterface<ISpeckleConverter>& )
{
	return ConvertToNativeInternal(Object, World);
}

UObject* UAggregateConverter::ConvertToNativeInternal(const UBase* Object, UWorld* World)
{
	check(IsInGameThread());
	
	if(!IsValid(Object)) return nullptr;
	
	const TSubclassOf<UBase> Type = Object->GetClass();
	UObject* Converter = GetConverter(Type).GetObject();
	if(Converter == nullptr)
	{
		if(Type != UBase::StaticClass())
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Skipping Object %s: No conversion functions exist for %s"), *Object->Id, *Type->GetName());
		}
		return nullptr;
	}

	UE_LOG(LogSpeckle, Log, TEXT("Converting object of type: %s id: %s"), *Type->GetName(), *Object->Id);
	
	FEditorScriptExecutionGuard ScriptGuard;
	
	TScriptInterface<ISpeckleConverter> MainConverter = this;
	return Execute_ConvertToNative(Converter, Object, World, MainConverter);

}

bool UAggregateConverter::CanConvertToNative_Implementation(TSubclassOf<UBase> BaseType)
{
	return GetConverter(BaseType).GetInterface() != nullptr;
}

TScriptInterface<ISpeckleConverter> UAggregateConverter::GetConverter(const TSubclassOf<UBase> BaseType)
{
	// Check if this SpeckleType has a known converter.
	if(SpeckleTypeMap.Contains(BaseType))
	{
		return SpeckleTypeMap[BaseType];
	}

	// Try and find one that can convert this SpeckleType.
	FEditorScriptExecutionGuard ScriptGuard;
	for(UObject* Converter : SpeckleConverters)
	{
		if(!CheckValidConverter(Converter)) continue;
		
		if(Execute_CanConvertToNative(Converter, BaseType))
		{ 
			//Found a Converter! Save this mapping for next time.
			SpeckleTypeMap.Add(BaseType, Converter);
			return Converter;
		}
	}
	
	// SpeckleType has no conversions.
	SpeckleTypeMap.Add(BaseType, nullptr);
	return nullptr;
}

void UAggregateConverter::CleanUp_Implementation()
{
	CleanUpInternal();
}

void UAggregateConverter::CleanUpInternal()
{
	for (UObject* Converter : SpeckleConverters)
	{
		if(!CheckValidConverter(Converter)) continue;
		
		Execute_CleanUp(Converter);
	}
}

bool UAggregateConverter::CheckValidConverter(const UObject* Converter, bool LogWarning)
{
	if(Converter == nullptr) return false;
	
	if(Converter->Implements<USpeckleConverter>()) return true;
	
	if(LogWarning)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface {%s}"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
	}
	
	return false;
}
