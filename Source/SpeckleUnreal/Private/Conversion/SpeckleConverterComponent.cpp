// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "API/SpeckleSerializer.h"
#include "Conversion/Converters/BlockConverter.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"


// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	//TODO consider using an object library for default converters
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> MeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	static ConstructorHelpers::FObjectFinder<UPointCloudConverter> PointCloudConverter(TEXT("PointCloudConverter'/SpeckleUnreal/Converters/DefaultPointCloudConverter.DefaultPointCloudConverter'"));
	static ConstructorHelpers::FObjectFinder<UBlockConverter> BlockConverter(TEXT("BlockConverter'/SpeckleUnreal/Converters/DefaultBlockConverter.DefaultBlockConverter'"));
	static ConstructorHelpers::FObjectFinder<UMaterialConverter> MaterialConverter(TEXT("MaterialConverter'/SpeckleUnreal/Converters/DefaultMaterialConverter.DefaultMaterialConverter'"));
	//static ConstructorHelpers::FObjectFinder<UCameraConverter> CameraConverter(TEXT("CameraConverter'/SpeckleUnreal/Converters/DefaultCameraConverter.DefaultCameraConverter'"));
	//static ConstructorHelpers::FObjectFinder<ULightConverter> LightConverter(TEXT("LightConverter'/SpeckleUnreal/Converters/DefaultLightConverter.DefaultLightConverter'"));

	SpeckleConverters = CreateDefaultSubobject<UAggregateConverter>(TEXT("Objects Converter"));
	
	SpeckleConverters->SpeckleConverters.Add(MeshConverter.Object);
	SpeckleConverters->SpeckleConverters.Add(PointCloudConverter.Object);
	SpeckleConverters->SpeckleConverters.Add(BlockConverter.Object);
	SpeckleConverters->SpeckleConverters.Add(MaterialConverter.Object);
	//SpeckleConverter->SpeckleConverters.Add(CameraConverter.Object);
	//SpeckleConverter->SpeckleConverters.Add(LightConverter.Object);
	
	PrimaryComponentTick.bCanEverTick = false;
}





void USpeckleConverterComponent::ConvertChildren(AActor* AOwner, const UBase* Base, const TScriptInterface<ITransport>& LocalTransport, TArray<AActor*>& OutActors)
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
                                                               const TScriptInterface<ITransport>& LocalTransport,
                                                               TArray<AActor*>& OutActors)
{
	if(!IsValid(Base)) return nullptr;
	
	// Convert Base
	UObject* Converted = SpeckleConverters->ConvertToNativeInternal(Base, AOwner->GetWorld());
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

void USpeckleConverterComponent::CleanUp()
{
	SpeckleConverters->CleanUpInternal();
}