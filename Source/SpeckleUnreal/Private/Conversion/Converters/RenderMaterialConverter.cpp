// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/Converters/RenderMaterialConverter.h"

#include "SpeckleUnrealManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Objects/RenderMaterial.h"


URenderMaterialConverter::URenderMaterialConverter()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleMaterial(TEXT("Material'/SpeckleUnreal/SpeckleMaterial.SpeckleMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SpeckleGlassMaterial(TEXT("Material'/SpeckleUnreal/SpeckleGlassMaterial.SpeckleGlassMaterial'"));

	DefaultMeshMaterial = SpeckleMaterial.Object;
	BaseMeshOpaqueMaterial = SpeckleMaterial.Object;
	BaseMeshTransparentMaterial = SpeckleGlassMaterial.Object;
}


bool URenderMaterialConverter::TryGetOverride(const URenderMaterial* SpeckleMaterial, UMaterialInterface*& OutMaterial) const
{
	const auto MaterialID = SpeckleMaterial->Id;
	

	//Override by id
	if(MaterialOverridesById.Contains(MaterialID))
	{
		OutMaterial = MaterialOverridesById[MaterialID];
		return true;
	}
	//Override by name
	const FString Name = SpeckleMaterial->Name;
	for (const UMaterialInterface* Mat : MaterialOverridesByName)
	{
		if(Mat->GetName() == Name)
		{
			OutMaterial = MaterialOverridesById[MaterialID];
			return true;
		}
	}
	
	
	return false;
}


UMaterialInterface* URenderMaterialConverter::GetMaterial(const URenderMaterial* SpeckleMaterial, bool AcceptMaterialOverride, bool UseEditorConstMaterial)
{
	if(SpeckleMaterial == nullptr || SpeckleMaterial->Id == "") return DefaultMeshMaterial; //Material is invalid

	// 1. Check Overrides
	UMaterialInterface* NativeMaterial;
	if(AcceptMaterialOverride && TryGetOverride(SpeckleMaterial, NativeMaterial))
		return NativeMaterial;

	// 2. Check transient cache
	if(ConvertedMaterials.Contains(SpeckleMaterial->Id))
	{
		return ConvertedMaterials[SpeckleMaterial->Id];
	}
	
	// 3. Check Assets
	UPackage* Package = GetPackage(SpeckleMaterial->Id);
	
	NativeMaterial = Cast<UMaterialInterface>(Package->FindAssetInPackage());
	if(IsValid(NativeMaterial))
	{
	   return NativeMaterial;
	}
	
	// 4. Convert
	return RenderMaterialToNative(SpeckleMaterial, Package, UseEditorConstMaterial);
}

UMaterialInterface* URenderMaterialConverter::RenderMaterialToNative(const URenderMaterial* SpeckleMaterial, UPackage* Package, bool UseEditorConstMaterial)
{
	UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
	    ? BaseMeshOpaqueMaterial
	    : BaseMeshTransparentMaterial;
	
	UMaterialInstance* MaterialInstance;
#if WITH_EDITOR
	if (UseEditorConstMaterial && GIsEditor)
	{
		const FName Name = MakeUniqueObjectName(Package, UMaterialInstanceConstant::StaticClass(), FName(SpeckleMaterial->Name));

		//TStrongObjectPtr< UMaterialInstanceConstantFactoryNew > MaterialFact( NewObject< UMaterialInstanceConstantFactoryNew >() );
		//MaterialFact->InitialParent = MaterialBase;
		//UMaterialInstanceConstant* ConstMaterial = Cast< UMaterialInstanceConstant >( MaterialFact->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, Name, RF_Public, nullptr, GWarn ) );
		UMaterialInstanceConstant* ConstMaterial = NewObject<UMaterialInstanceConstant>(Package, Name, RF_Public);
		
		MaterialInstance = ConstMaterial;
		ConstMaterial->SetParentEditorOnly(MaterialBase);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Opacity"), SpeckleMaterial->Opacity);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Metallic"), SpeckleMaterial->Metalness);
		ConstMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo("Roughness"), SpeckleMaterial->Roughness);
		ConstMaterial->SetVectorParameterValueEditorOnly(FMaterialParameterInfo("BaseColor"), SpeckleMaterial->Diffuse);
		ConstMaterial->SetVectorParameterValueEditorOnly(FMaterialParameterInfo("EmissiveColor"), SpeckleMaterial->Emissive);
		
		//ConstMaterial->InitStaticPermutation();
		
		ConstMaterial->MarkPackageDirty();

		FAssetRegistryModule::AssetCreated(MaterialInstance);
	}
	else
#endif
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialBase, Package, FName(SpeckleMaterial->Name));
		MaterialInstance = DynMaterial;
		
		DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
		DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
		DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
		DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
		DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
		
		DynMaterial->SetFlags(RF_Public);
	}
	
	ConvertedMaterials.Add(SpeckleMaterial->Id, MaterialInstance);
	
	return MaterialInstance;
	
}

void URenderMaterialConverter::CleanUp()
{
	ConvertedMaterials.Empty();
}

UPackage* URenderMaterialConverter::GetPackage(const FString& ObjectID ) const
{
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle/Materials"), ObjectID);
	return CreatePackage(*PackagePath);
}
