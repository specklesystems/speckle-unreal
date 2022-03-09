// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "LogSpeckle.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/Converters/BlockConverter.h"
#include "Conversion/Converters/DisplayValueConverter.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"


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


UObject* USpeckleConverterComponent::ConvertToNative(const UBase* Object, UWorld* World)
{
	check(IsInGameThread());
	
	if(!IsValid(Object)) return nullptr;
	
	const TSubclassOf<UBase> Type = Object->GetClass();
	UObject* Converter = GetConverter(Type).GetObject();
	if(Converter == nullptr)
	{
	    if(Type != UBase::StaticClass())
	    {
		    UE_LOG(LogSpeckle, Warning, TEXT("Skipping Object %s: No actor conversion functions exist for %s"), *Object->Id, *Type->GetName());
	    }
		return nullptr;
	}

	UE_LOG(LogSpeckle, Log, TEXT("Converting object of type: %s id: %s  "), *Type->GetName(), *Object->Id);
	
	FEditorScriptExecutionGuard ScriptGuard;
	return ISpeckleConverter::Execute_ConvertToNative(Converter, Object, World);

}


void USpeckleConverterComponent::ConvertChildren(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport> LocalTransport, TArray<AActor*>& OutActors)
{
	//Convert Children
	TMap<FString, TSharedPtr<FJsonValue>> PotentialChildren = Base->DynamicProperties;
	
	for (const auto& Kv : PotentialChildren)
	{
		const TSharedPtr<FJsonObject>* SubObjectPtr;
		if (Kv.Value->TryGetObject(SubObjectPtr))
		{
			const UBase* Child = USpeckleSerializer::DeserializeBase(*SubObjectPtr, LocalTransport);
			RecursivelyConvertToNative(AOwner, Child, LocalTransport,OutActors);
			continue;
		}

		const TArray<TSharedPtr<FJsonValue>>* SubArrayPtr;
		if (Kv.Value->TryGetArray(SubArrayPtr))
		{
			for (const auto ArrayElement : *SubArrayPtr)
			{
				const TSharedPtr<FJsonObject>* ArraySubObjPtr;
				if (!ArrayElement->TryGetObject(ArraySubObjPtr)) continue;
				
				const UBase* Child = USpeckleSerializer::DeserializeBase(*ArraySubObjPtr, LocalTransport);
				RecursivelyConvertToNative(AOwner, Child, LocalTransport,OutActors);
			}
		}
	}
}

AActor* USpeckleConverterComponent::RecursivelyConvertToNative(AActor* AOwner, const UBase* Base,
                                                               const TScriptInterface<ITransport> LocalTransport,
                                                               TArray<AActor*>& OutActors)
{
	if(!IsValid(Base)) return nullptr;
	
	// Convert Base
	UObject* Converted = ConvertToNative(Base, AOwner->GetWorld());
	AActor* Owner = AOwner;
		
	if(IsValid(Converted))
	{
		// Handle Converted Object being an Actor
		AActor* NativeActor = Cast<AActor>(Converted);
		if(IsValid(NativeActor))
		{
#if WITH_EDITOR
			NativeActor->SetActorLabel(FString::Printf(TEXT("%s - %s"), *Base->SpeckleType, *Base->Id));
#endif
		
			// Ensure actor has a valid mobility for it's owner
			if(NativeActor->HasValidRootComponent())
			{
				uint8 CurrentMobility = NativeActor->GetRootComponent()->Mobility;
				uint8 OwnerMobility = AOwner->GetRootComponent()->Mobility;
			
				if(CurrentMobility < OwnerMobility)
				{
					NativeActor->GetRootComponent()->SetMobility(AOwner->GetRootComponent()->Mobility);
				}
			}
		
			NativeActor->AttachToActor(AOwner, FAttachmentTransformRules::KeepRelativeTransform);
			NativeActor->SetOwner(AOwner);

			OutActors.Add(NativeActor);
			Owner = NativeActor;
		}

		// Handle Converted Object being a Component
		UActorComponent* NativeComponent = Cast<UActorComponent>(Converted);
		if(IsValid(NativeComponent))
		{
			if(!Owner->HasValidRootComponent()) Owner->SetRootComponent(NewObject<USceneComponent>(Owner));

			USceneComponent* SceneComponent = Cast<USceneComponent>(Converted);
			if(IsValid(SceneComponent)) SceneComponent->SetupAttachment(Owner->GetRootComponent());

			NativeComponent->RegisterComponent();
		}
		
	}
	
	ConvertChildren(Owner, Base, LocalTransport, OutActors);
	
	return Owner;
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

void USpeckleConverterComponent::CleanUp()
{
	for (UObject* Converter : SpeckleConverters)
	{
		if(!CheckValidConverter(Converter)) continue;
		
		ISpeckleConverter::Execute_CleanUp(Converter);
	}
}

bool USpeckleConverterComponent::CheckValidConverter(const UObject* Converter, bool LogWarning)
{
	if(Converter == nullptr) return false;
		
	if(Converter->GetClass()->ImplementsInterface(USpeckleConverter::StaticClass())) return true;
	
	if(LogWarning)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Converter {%s} is not a valid converter, Expected to implement interface {%s}"), *Converter->GetClass()->GetName(), *USpeckleConverter::StaticClass()->GetName())
	}
	
	return false;
}
