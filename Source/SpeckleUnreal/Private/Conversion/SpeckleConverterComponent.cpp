// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"
#include "Objects/Mesh.h"
#include "LogSpeckle.h"


// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	//TODO consider using an object library for default converters
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> MeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	static ConstructorHelpers::FObjectFinder<UPointCloudConverter> PointCloudConverter(TEXT("PointCloudConverter'/SpeckleUnreal/Converters/DefaultPointCloudConverter.DefaultPointCloudConverter'"));

	PrimaryComponentTick.bCanEverTick = false;



	
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
		if(Converter != nullptr && !Converter->GetClass()->ImplementsInterface(USpeckleConverter::StaticClass()))
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface %s"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
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
	if(!IsValid(Object)) return nullptr;
	
	const TSubclassOf<UBase> Type = Object->GetClass();
	UObject* Converter = GetConverter(Type).GetObject();
	if(Converter == nullptr)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Skipping Object %s: No actor conversion functions exist for %s"), *Object->Id, *Type->GetName());
		return nullptr;
	}

	UE_LOG(LogSpeckle, Log, TEXT("Converting object of type: %s id: %s  "), *Type->GetName(), *Object->Id);
	
	FEditorScriptExecutionGuard ScriptGuard;
	return ISpeckleConverter::Execute_ConvertToNative(Converter, Object, Manager);

}

TScriptInterface<ISpeckleConverter> USpeckleConverterComponent::GetConverter(const TSubclassOf<UBase> BaseType)
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
		
		if(!Converter->GetClass()->ImplementsInterface(USpeckleConverter::StaticClass()))
		{
			UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface {%s}"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
			continue;
		}
		
		if(ISpeckleConverter::Execute_CanConvertToNative(Converter, BaseType))
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