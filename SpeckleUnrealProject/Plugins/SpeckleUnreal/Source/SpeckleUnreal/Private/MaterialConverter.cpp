// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialConverter.h"
#include "RenderMaterial.h"

void UMaterialConverter::AssignPropertiesFromSpeckle(UMaterialInstanceDynamic* Material, const URenderMaterial* SpeckleMaterial)
{
	Material->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
	Material->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
	Material->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
	Material->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
	Material->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
}

URenderMaterial* UMaterialConverter::ParseRenderMaterial(const TSharedPtr<FJsonObject> obj)
{
	URenderMaterial* RenderMaterial = NewObject<URenderMaterial>();;
	
	obj->TryGetStringField("name", RenderMaterial->Name);
	obj->TryGetNumberField("opacity", RenderMaterial->Opacity);
	obj->TryGetNumberField("metalness", RenderMaterial->Metalness);
	obj->TryGetNumberField("roughness", RenderMaterial->Roughness);
	
	int32 ARGB;
	obj->TryGetNumberField("diffuse", ARGB);
		RenderMaterial->Diffuse = FColor(ARGB);
		
	if(obj->TryGetNumberField("emissive", ARGB))
		RenderMaterial->Emissive = FColor(ARGB);
	
	return RenderMaterial;
}
