// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SpeckleConverterComponent.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/TypeConverters/PointCloudConverter.h"
#include "Conversion/TypeConverters/StaticMeshConverter.h"
#include "Objects/Mesh.h"
#include "Objects/PointCloud.h"


// Sets default values for this component's properties
USpeckleConverterComponent::USpeckleConverterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SpeckleConverters.Add(UMesh::StaticClass(), CreateDefaultSubobject<UStaticMeshConverter>(FName("Static Mesh Converter")));
	SpeckleConverters.Add(UPointCloud::StaticClass(), CreateDefaultSubobject<UPointCloudConverter>(FName("Point Cloud Converter")));
	//SpeckleConverter.Add(ULight, FLightConverter);
}

UBase* USpeckleConverterComponent::ConvertToSpeckle(UObject* Object)
{
	//TODO implement ToSpeckle
	return nullptr;
}

bool USpeckleConverterComponent::CanConvertToSpeckle(const UObject* Object) const
{
	//TODO implement ToSpeckle
	return false;
}

AActor* USpeckleConverterComponent::ConvertToNative(const UBase* Object, ASpeckleUnrealManager* Manager)
{
	const TSubclassOf<UBase> Type = Object->GetClass();
	if(!CanConvertToNative(Type))
	{
		UE_LOG(LogTemp, Warning, TEXT("Object %s failed conversion - No conversion functions exist for %s"), *Object->Id, *Type->GetName());
		return nullptr;
	}
	
	if(!SpeckleConverters[Type]->GetClass()->ImplementsInterface(USpeckleTypeConverter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s does not implement $s interface"), *SpeckleConverters[Type]->GetName(), USpeckleTypeConverter::StaticClass());
		return nullptr;
	}

	FEditorScriptExecutionGuard ScriptGuard;
	return ISpeckleTypeConverter::Execute_ConvertToNative(SpeckleConverters[Type], Object, Manager);

}

bool USpeckleConverterComponent::CanConvertToNative(const TSubclassOf<UBase> Type) const
{
	return SpeckleConverters.Contains(Type);
}