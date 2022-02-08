// Fill out your copyright notice in the Description page of Project Settings.


#include "Conversion/Converters/RenderMaterialConverter.h"

#include "SpeckleUnrealManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Objects/RenderMaterial.h"


URenderMaterialConverter::URenderMaterialConverter()
{
	//DefaultMeshMaterial
}

bool URenderMaterialConverter::TryGetExistingMaterial(const URenderMaterial* SpeckleMaterial, const bool AcceptMaterialOverride, UMaterialInterface*& OutMaterial) const
{
	const auto MaterialID = SpeckleMaterial->Id;
	
	if(AcceptMaterialOverride)
	{
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
	}
	
	if(ConvertedMaterials.Contains(MaterialID))
	{
		OutMaterial = ConvertedMaterials[MaterialID];
		return true;
	}

	return false;
}

UMaterialInterface* URenderMaterialConverter::GetMaterial(const URenderMaterial* SpeckleMaterial, const ASpeckleUnrealManager* Manager, bool AcceptMaterialOverride, bool UseEditorConstMaterial)
{
	UMaterialInterface* NativeMaterial;
	if(TryGetExistingMaterial(SpeckleMaterial, AcceptMaterialOverride, NativeMaterial))
		return NativeMaterial;
	
	return RenderMaterialToNative(SpeckleMaterial, Manager, UseEditorConstMaterial);
}

UMaterialInterface* URenderMaterialConverter::RenderMaterialToNative(const URenderMaterial* SpeckleMaterial, const ASpeckleUnrealManager* Manager, bool UseEditorConstMaterial)
{
	if(SpeckleMaterial == nullptr || SpeckleMaterial->Id == "") return DefaultMeshMaterial; //Material is invalid

	
	UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
	    ? BaseMeshOpaqueMaterial
	    : BaseMeshTransparentMaterial;
	
	UPackage* Package = GetPackage(Manager->StreamID, SpeckleMaterial->Id);
	
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

UPackage* URenderMaterialConverter::GetPackage(const FString& StreamID, const FString& ObjectID ) const
{
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), StreamID, TEXT("Materials"), ObjectID);
	return CreatePackage(*PackagePath);
}