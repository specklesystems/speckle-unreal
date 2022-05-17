// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverterComponent.h"

#include "ActorEditorUtils.h"
#include "API/SpeckleSerializer.h"
#include "Conversion/Converters/AggregateConverter.h"
#include "Conversion/Converters/BlockConverter.h"
#include "Conversion/Converters/PointCloudConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"
#include "Conversion/Converters/MaterialConverter.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"

// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	//TODO consider using an object library for default converters
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> MeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	static ConstructorHelpers::FObjectFinder<UPointCloudConverter> PointCloudConverter(TEXT("PointCloudConverter'/SpeckleUnreal/Converters/DefaultPointCloudConverter.DefaultPointCloudConverter'"));
	static ConstructorHelpers::FObjectFinder<UBlockConverter> BlockConverter(TEXT("BlockConverter'/SpeckleUnreal/Converters/DefaultBlockConverter.DefaultBlockConverter'"));
	static ConstructorHelpers::FObjectFinder<UMaterialConverter> MaterialConverter(TEXT("MaterialConverter'/SpeckleUnreal/Converters/DefaultMaterialConverter.DefaultMaterialConverter'"));
	static ConstructorHelpers::FObjectFinder<UObject> CameraConverter(TEXT("CameraConverter'/SpeckleUnreal/Converters/DefaultCameraConverter.DefaultCameraConverter'"));
	//static ConstructorHelpers::FObjectFinder<ULightConverter> LightConverter(TEXT("LightConverter'/SpeckleUnreal/Converters/DefaultLightConverter.DefaultLightConverter'"));

	SpeckleConverter = CreateDefaultSubobject<UAggregateConverter>(TEXT("Objects Converter"));
	
	SpeckleConverter->SpeckleConverters.Add(MeshConverter.Object);
	SpeckleConverter->SpeckleConverters.Add(PointCloudConverter.Object);
	SpeckleConverter->SpeckleConverters.Add(BlockConverter.Object);
	SpeckleConverter->SpeckleConverters.Add(MaterialConverter.Object);
	SpeckleConverter->SpeckleConverters.Add(CameraConverter.Object);
	//SpeckleConverter->SpeckleConverters.Add(LightConverter.Object);
	
	PrimaryComponentTick.bCanEverTick = false;
}

AActor* USpeckleConverterComponent::RecursivelyConvertToNative(AActor* AOwner, const UBase* Base,
	const TScriptInterface<ITransport>& LocalTransport, bool DisplayProgressBar, TArray<AActor*>& OutActors)
{
	float ObjectsToConvert{};
	Base->TryGetDynamicNumber("totalChildrenCount", ObjectsToConvert);
	FScopedSlowTask Progress(ObjectsToConvert + 2, LOCTEXT("SpeckleConvertoNative","Converting Speckle Objects to Native"), DisplayProgressBar);
#if WITH_EDITOR
	Progress.MakeDialog(true, false);
#endif
	
	AActor* RootActor = RecursivelyConvertToNative_Internal(AOwner, Base, LocalTransport, &Progress, OutActors);
	
	FinishConversion();
	return RootActor;
}

AActor* USpeckleConverterComponent::RecursivelyConvertToNative_Internal(AActor* AOwner, const UBase* Base,
                                                                        const TScriptInterface<ITransport>& LocalTransport,
                                                                        FSlowTask* Task,
                                                                        TArray<AActor*>& OutActors)
{
	check(IsValid(AOwner));
	if(!IsValid(Base)) return nullptr;
	
	// Convert Speckle Object
	UObject* Converted = SpeckleConverter->ConvertToNativeInternal(Base, AOwner->GetWorld());
	AttachConvertedToOwner(AOwner, Base, Converted);

	// Handle new actors
	AActor* ConvertedAsActor = Cast<AActor>(Converted);
	AActor* NextOwner =  IsValid(ConvertedAsActor) ? ConvertedAsActor : AOwner;
	if(NextOwner != AOwner)
	{
		OutActors.Add(NextOwner);
		OutActors.Append(NextOwner->Children);
	}

	Task->EnterProgressFrame(1);
	if(Task->ShouldCancel()) return AOwner;
	
	//Convert Children
	TMap<FString, TSharedPtr<FJsonValue>> PotentialChildren = Base->DynamicProperties;
	
	for (const auto& Kvp : PotentialChildren)
	{
		if(Task->ShouldCancel()) break;
		
		ConvertChild(Kvp.Value, AOwner, LocalTransport, Task, OutActors);
	}
	return AOwner;
}

void USpeckleConverterComponent::ConvertChild(const TSharedPtr<FJsonValue> Object, AActor* AOwner,
                                              const TScriptInterface<ITransport>& LocalTransport, FSlowTask* Task,
                                              TArray<AActor*>& OutActors)
{
	//Handle child object
	const TSharedPtr<FJsonObject>* ChildObj;
	if (Object->TryGetObject(ChildObj))
	{
		const UBase* Child = USpeckleSerializer::DeserializeBase(*ChildObj, LocalTransport);
		RecursivelyConvertToNative_Internal(AOwner, Child, LocalTransport, Task, OutActors);
		return;
	}

	//Handle child array object
	const TArray<TSharedPtr<FJsonValue>>* ChildArr;
	if (Object->TryGetArray(ChildArr))
	{
		for (const auto v : *ChildArr)
		{
			ConvertChild(v, AOwner, LocalTransport, Task, OutActors);
		}
	}
};
	

void USpeckleConverterComponent::AttachConvertedToOwner(AActor* AOwner, const UBase* Base, UObject* Converted)
{
	
	// Case Actor
	{
		AActor* NativeActor = Cast<AActor>(Converted);
		if(IsValid(NativeActor))
		{
	#if WITH_EDITOR
			{
				FString Name;
				FText _Discard;
				if( !(Base->TryGetDynamicString("name", Name) && FActorEditorUtils::ValidateActorName(FText::FromString(Name), _Discard)) )
				{
					Name = FString::Printf(TEXT("%s - %s"), *Base->SpeckleType, *Base->Id);
				}
				NativeActor->SetActorLabel(Name);
			}
	#endif
		
			// Ensure actor has a valid mobility for its owner
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
			return;
		}
	}

	// Case ActorComponent
	{
		UActorComponent* NativeComponent = Cast<UActorComponent>(Converted);
		if(IsValid(NativeComponent))
		{
			if(!AOwner->HasValidRootComponent()) AOwner->SetRootComponent(NewObject<USceneComponent>(AOwner));

			USceneComponent* SceneComponent = Cast<USceneComponent>(Converted);
			if(IsValid(SceneComponent)) SceneComponent->SetupAttachment(AOwner->GetRootComponent());

			NativeComponent->RegisterComponent();
			return;
		}
	}
	
}


void USpeckleConverterComponent::FinishConversion()
{
	SpeckleConverter->FinishConversion_Internal();
}

#undef LOCTEXT_NAMESPACE