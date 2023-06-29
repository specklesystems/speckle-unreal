
#include "Conversion/Converters/MaterialConverter.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Objects/Other/RenderMaterial.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"


UMaterialConverter::UMaterialConverter()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SpeckleMaterial(TEXT("Material'/SpeckleUnreal/SpeckleMaterial.SpeckleMaterial'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SpeckleGlassMaterial(TEXT("Material'/SpeckleUnreal/SpeckleGlassMaterial.SpeckleGlassMaterial'"));

	DefaultMeshMaterial = SpeckleMaterial.Object;
	BaseMeshOpaqueMaterial = SpeckleMaterial.Object;
	BaseMeshTransparentMaterial = SpeckleGlassMaterial.Object;
	
#if WITH_EDITORONLY_DATA
	UseConstMaterials = NotPlay;
#endif
	
	SpeckleTypes.Add(URenderMaterial::StaticClass());
}

UObject* UMaterialConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, UWorld*, TScriptInterface<ISpeckleConverter>&)
{
	const URenderMaterial* m = Cast<URenderMaterial>(SpeckleBase);
	
	if(m == nullptr) return DefaultMeshMaterial;
	
	return GetMaterial(m);
}

UMaterialInterface* UMaterialConverter::GetMaterial(const URenderMaterial* SpeckleMaterial)
{
	if(SpeckleMaterial == nullptr || SpeckleMaterial->Id == "") return DefaultMeshMaterial; //Material is invalid
	
	// 1. Check Overrides
	UMaterialInterface* NativeMaterial;
	if(TryGetOverride(SpeckleMaterial, NativeMaterial))
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
	return RenderMaterialToNative(SpeckleMaterial, Package);
}

bool UMaterialConverter::TryGetOverride(const URenderMaterial* SpeckleMaterial, UMaterialInterface*& OutMaterial) const
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
		if(ensureAlways(IsValid(Mat)) && Mat->GetName() == Name)
		{
			OutMaterial = *MaterialOverridesByName.Find(Mat);
			return true;
		}
	}
	
	
	return false;
}

FString UMaterialConverter::RemoveInvalidFileChars(const FString& InString) const
{
	return FPaths::MakeValidFileName(InString.Replace(TEXT("."), TEXT("_"), ESearchCase::CaseSensitive));
}

UMaterialInterface* UMaterialConverter::RenderMaterialToNative(const URenderMaterial* SpeckleMaterial, UPackage* Package)
{
	UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
	    ? BaseMeshOpaqueMaterial
	    : BaseMeshTransparentMaterial;
	
	UMaterialInstance* MaterialInstance;
#if WITH_EDITOR
	if (ShouldCreateConstMaterial(UseConstMaterials))
	{
		const FName Name = MakeUniqueObjectName(Package, UMaterialInstanceConstant::StaticClass(), *RemoveInvalidFileChars(SpeckleMaterial->Name));

		//TStrongObjectPtr< UMaterialInstanceConstantFactoryNew > MaterialFact( NewObject< UMaterialInstanceConstantFactoryNew >() );
		//MaterialFact->InitialParent = MaterialBase;
		//UMaterialInstanceConstant* ConstMaterial = Cast< UMaterialInstanceConstant >( MaterialFact->FactoryCreateNew( UMaterialInstanceConstant::StaticClass(), Package, Name, RF_Public, nullptr, GWarn ) );
		UMaterialInstanceConstant* ConstMaterial = NewObject<UMaterialInstanceConstant>(Package, Name, RF_Public | RF_Standalone);
		
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

void UMaterialConverter::FinishConversion_Implementation()
{
	ConvertedMaterials.Empty();
}

UPackage* UMaterialConverter::GetPackage(const FString& ObjectID ) const
{
	const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle/Materials"), ObjectID);
	return CreatePackage(*PackagePath);
}


#if WITH_EDITOR
bool UMaterialConverter::ShouldCreateConstMaterial(TEnumAsByte<EConstMaterialOptions> Options)
{
	if(!GIsEditor) return false;
	
	switch(Options)
	{
		case Never:
			return false;
		case NotPlay:
			return !FApp::IsGame();
		case Always:
			return true;
		default:
			unimplemented();
			return false;
	}
}
#endif