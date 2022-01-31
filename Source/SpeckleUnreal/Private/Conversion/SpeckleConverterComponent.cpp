// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"


// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	//TODO consider using an object library for default converters
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> MeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	static ConstructorHelpers::FObjectFinder<UPointCloudConverter> PointCloudConverter(TEXT("PointCloudConverter'/SpeckleUnreal/Converters/DefaultPointCloudConverter.DefaultPointCloudConverter'"));
	
	//Mesh
	//Point
	//Light
	//BlockDef
	//BlockInst
	//Wall
	//Element
	
	SpeckleConverters.Add(MeshConverter.Object);
	SpeckleConverters.Add(PointCloudConverter.Object);
	//SpeckleConverter.Add(FLightConverter);
}

void USpeckleConverterComponent::OnConvertersChangeHandler()
{
	SpeckleTypeMap.Empty();

	for(int i = 0; i < SpeckleConverters.Num(); i++)
	{
		const UObject* Converter = SpeckleConverters[i];
		if(Converter != nullptr && !Converter->GetClass()->ImplementsInterface(USpeckleTypeConverter::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface %s"), *Converter->GetClass()->GetName(), *USpeckleTypeConverter::StaticClass()->GetName())
			SpeckleConverters.RemoveAt(i);
			i--;
		}
	}
}

#if WITH_EDITOR
void USpeckleConverterComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(USpeckleConverterComponent, SpeckleConverters))
	{
		OnConvertersChangeHandler();
	}
}
#endif

UBase* USpeckleConverterComponent::ConvertToSpeckle(UObject* Object)
{
	//TODO implement ToSpeckle
	return nullptr;
}


AActor* USpeckleConverterComponent::ConvertToNative(const UBase* Object, ASpeckleUnrealManager* Manager)
{
	check(Object != nullptr);
	const TSubclassOf<UBase> Type = Object->GetClass();
	UObject* Converter = GetConverter(Type).GetObject();
	if(Converter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping Object %s - No conversion functions exist for %s"), *Object->Id, *Type->GetName());
		return nullptr;
	}
	
	FEditorScriptExecutionGuard ScriptGuard;
	AActor* ReturnObject = ISpeckleTypeConverter::Execute_ConvertToNative(Converter, Object, Manager);
	
	UE_LOG(LogTemp, Log, TEXT("Converted object of type: %s id: %s  "), *Object->Id, *Type->GetName());
	
	return ReturnObject;
}

TScriptInterface<ISpeckleTypeConverter> USpeckleConverterComponent::GetConverter(const TSubclassOf<UBase> BaseType)
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
		if(Converter == nullptr) continue;
		
		if(!Converter->GetClass()->ImplementsInterface(USpeckleTypeConverter::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface {%s}"), *Converter->GetClass()->GetName(), *USpeckleTypeConverter::StaticClass()->GetName())
			continue;
		}
		
		if(ISpeckleTypeConverter::Execute_CanConvertToNative(Converter, BaseType))
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