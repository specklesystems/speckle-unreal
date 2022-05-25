// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/SpeckleConverter.h"
#include "Conversion/SpeckleConverterComponent.h"
#include "Conversion/Converters/AggregateConverter.h"


#define LOCTEXT_NAMESPACE "FSpeckleUnrealModule"


UBase* USpeckleConverterComponent::RecursivelyConvertToSpeckle(const TArray<AActor*>& RootActors, FActorPredicate& Predicate)
{
	TArray<UBase*> ConvertedRootActors;
	for(const AActor* RootActor : RootActors)
	{
		RecurseTreeToSpeckle(RootActor, Predicate, ConvertedRootActors);
	}

	UBase* Wrapper = NewObject<UBase>(GetTransientPackage(), NAME_None, RF_Transient);

	//Converted->DynamicProperties["objects"] = ConvertedRootActors; //TODO set children
	
	return Wrapper;
}

void USpeckleConverterComponent::RecurseTreeToSpeckle(const AActor* RootActor, FActorPredicate& Predicate, TArray<UBase*>& OutConverted)
{
	// Convert children first
	TArray<UBase*> ConvertedChildren;
	ConvertedChildren.Reserve(RootActor->Children.Num());
	for (const AActor* Child : RootActor->Children)
	{
		RecurseTreeToSpeckle(Child, Predicate, ConvertedChildren);
	}

	bool ShouldConvert; 
	Predicate.Execute(RootActor, ShouldConvert);
	if(ISpeckleConverter::Execute_CanConvertToSpeckle(SpeckleConverter, RootActor) && ShouldConvert)
	{
		// Convert and output
		UBase* Converted = NewObject<UBase>(GetTransientPackage(), NAME_None, RF_Transient);
		//Converted->DynamicProperties["name"] = RootActor->GetName(),
		//["transform"] = TransformToSpeckle(go.Transform), //TODO set common props
		//Converted->DynamicProperties["tag"] = go.tag,
		//Converted->DynamicProperties["layer"] = go.layer,
		//Converted->DynamicProperties["isStatic"] = go.isStatic,
		
		ISpeckleConverter::Execute_ConvertToSpeckle(SpeckleConverter, RootActor, Converted);
		//Converted->DynamicProperties["objects"] = ConvertedChildren; //TODO set children
		OutConverted.Add(Converted);
	}
	else
	{
		// Skip this object, and output any children
		OutConverted.Append(ConvertedChildren);
	}
	
}
#undef LOCTEXT_NAMESPACE
