// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/Converters/DisplayValueConverter.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/Converters/RenderMaterialConverter.h"
#include "Conversion/Converters/StaticMeshConverter.h"
#include "Objects/DisplayValueElement.h"
#include "Objects/Mesh.h"

UDisplayValueConverter::UDisplayValueConverter()
{
	static ConstructorHelpers::FObjectFinder<UStaticMeshConverter> DefaultMeshConverter(TEXT("StaticMeshConverter'/SpeckleUnreal/Converters/DefaultStaticMeshConverter.DefaultStaticMeshConverter'"));
	MeshConverter = DefaultMeshConverter.Object;
	
	SpeckleTypes.Add(UDisplayValueElement::StaticClass());
}

AActor* UDisplayValueConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager)
{

	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Geometry"), SpeckleBase->Id);
	UPackage* Package = CreatePackage(*PackagePath);

	const UDisplayValueElement* SpeckleElement = Cast<UDisplayValueElement>(SpeckleBase);
	if(SpeckleElement == nullptr) return nullptr;

	//Find existing mesh
	UStaticMesh* Mesh = Cast<UStaticMesh>(Package->FindAssetInPackage());

	if(!IsValid(Mesh))
	{
		//No existing mesh was found, try and convert SpeckleMesh
		Mesh = MeshConverter->MeshesToNative(Package, SpeckleElement, SpeckleElement->DisplayValue, Manager);
	}

	AActor* Actor = MeshConverter->CreateEmptyActor(Manager);
	TInlineComponentArray<UStaticMeshComponent*> Components;
	Actor->GetComponents<UStaticMeshComponent>(Components);

	UStaticMeshComponent* MeshComponent;
	if(Components.Num() > 0) MeshComponent = Components[0];
	else
	{
		// MeshActorType doesn't have a UStaticMeshComponent, so we will add one
		MeshComponent = NewObject<UStaticMeshComponent>(Actor, FName("SpeckleMeshComponent"));
		MeshComponent->SetupAttachment(Actor->GetRootComponent());
		MeshComponent->RegisterComponent();
	}

	MeshComponent->SetStaticMesh(Mesh);

	int i = 0;
	for(const UMesh* DisplayMesh : SpeckleElement->DisplayValue)
	{
		MeshComponent->SetMaterial(i, MeshConverter->MaterialConverter->GetMaterial(DisplayMesh->RenderMaterial, Manager, true, !FApp::IsGame()));
		i++;
	}

	return Actor;
	
}

UBase* UDisplayValueConverter::ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager)
{
	return nullptr; //TODO implement
}
