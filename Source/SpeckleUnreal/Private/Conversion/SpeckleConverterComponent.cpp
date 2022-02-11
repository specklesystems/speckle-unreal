// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"
#include "Objects/Mesh.h"
#include "LogSpeckle.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/Converters/BlockConverter.h"
#include "Conversion/Converters/DisplayValueConverter.h"


// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	//TODO consider using an object library for default converters
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> MeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	static ConstructorHelpers::FObjectFinder<UPointCloudConverter> PointCloudConverter(TEXT("PointCloudConverter'/SpeckleUnreal/Converters/DefaultPointCloudConverter.DefaultPointCloudConverter'"));
	static ConstructorHelpers::FObjectFinder<UDisplayValueConverter> DisplayValueConverter(TEXT("DisplayValueConverter'/SpeckleUnreal/Converters/DefaultDisplayValueConverter.DefaultDisplayValueConverter'"));
	static ConstructorHelpers::FObjectFinder<UBlockConverter> BlockConverter(TEXT("BlockConverter'/SpeckleUnreal/Converters/DefaultBlockConverter.DefaultBlockConverter'"));
	//static ConstructorHelpers::FObjectFinder<UCameraConverter> CameraConverter(TEXT("CameraConverter'/SpeckleUnreal/Converters/DefaultCameraConverter.DefaultCameraConverter'"));
	//static ConstructorHelpers::FObjectFinder<ULightConverter> LightConverter(TEXT("LightConverter'/SpeckleUnreal/Converters/DefaultLightConverter.DefaultLightConverter'"));

	SpeckleConverters.Add(MeshConverter.Object);
	SpeckleConverters.Add(PointCloudConverter.Object);
	SpeckleConverters.Add(BlockConverter.Object);
	//SpeckleConverter.Add(CameraConverter.Object);
	//SpeckleConverter.Add(LightConverter.Object);
	SpeckleConverters.Add(DisplayValueConverter.Object);
	
	PrimaryComponentTick.bCanEverTick = false;
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


AActor* USpeckleConverterComponent::ConvertToNative(const UBase* Object, UWorld* World)
{
	check(IsInGameThread());
	
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
	return ISpeckleConverter::Execute_ConvertToNative(Converter, Object, World);

}


AActor* USpeckleConverterComponent::RecursivelyConvertToNative(AActor* AOwner, const UBase* Base,
                                                               const TScriptInterface<ITransport> LocalTransport,
                                                               TArray<AActor*>& OutActors)
{
	if(!IsValid(Base)) return nullptr;
	
	// Convert Base
	AActor* Native = ConvertToNative(Base, AOwner->GetWorld());
	
	if(IsValid(Native))
	{

#if WITH_EDITOR
		Native->SetActorLabel(FString::Printf(TEXT("%s - %s"), *Base->SpeckleType, *Base->Id));
#endif
		
		Native->AttachToActor(AOwner, FAttachmentTransformRules::KeepRelativeTransform);
		Native->SetOwner(AOwner);

		OutActors.Add(Native);
	}
	else
	{
		Native = AOwner;
	}
	
	//Convert Children
	TMap<FString, TSharedPtr<FJsonValue>> PotentialChildren = Base->DynamicProperties;
	
	for (const auto& Kv : PotentialChildren)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Kv.Value->TryGetObject(SubObjectPtr))
		{
			const UBase* Child = FSpeckleSerializer::DeserializeBase(*SubObjectPtr, LocalTransport);
			RecursivelyConvertToNative(Native, Child, LocalTransport,OutActors);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (const auto ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (!ArrayElement->TryGetObject(ArraySubObjPtr)) continue;
				
				const UBase* Child = FSpeckleSerializer::DeserializeBase(*ArraySubObjPtr, LocalTransport);
				RecursivelyConvertToNative(Native, Child, LocalTransport,OutActors);
			}
		}
	}
	
	return Native;
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
		if(!CheckValidConverter(Converter)) continue;
		
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

void USpeckleConverterComponent::DeleteObjects()
{
	for (UObject* Converter : SpeckleConverters)
	{
		if(!CheckValidConverter(Converter)) continue;
		
		ISpeckleConverter::Execute_CleanUp(Converter);
	}
}

bool USpeckleConverterComponent::CheckValidConverter(const UObject* Converter)
{
	if(Converter == nullptr) return false;
		
	if(!Converter->GetClass()->ImplementsInterface(USpeckleConverter::StaticClass()))
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface {%s}"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
		return false;
	}
	return true;
}
