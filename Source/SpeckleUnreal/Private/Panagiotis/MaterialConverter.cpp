// Fill out your copyright notice in the Description page of Project Settings.


#include "Panagiotis/MaterialConverter.h"
#include "Objects/RenderMaterial.h"

void UMaterialConverter::AssignPropertiesFromSpeckle(UMaterialInstanceDynamic* Material, const URenderMaterial* SpeckleMaterial)
{

}

URenderMaterial* UMaterialConverter::ParseRenderMaterial(const TSharedPtr<FJsonObject> obj)
{
	URenderMaterial* RenderMaterial = NewObject<URenderMaterial>();;
	
	obj->TryGetStringField("id", RenderMaterial->ObjectID);
	obj->TryGetStringField("name", RenderMaterial->Name);
	obj->TryGetNumberField("opacity", RenderMaterial->Opacity);
	//obj->TryGetNumberField("metalness", RenderMaterial->Metalness);
	
	//obj->TryGetNumberField("roughness", RenderMaterial->Roughness);
	
	int32 ARGB;
	if(obj->TryGetNumberField("diffuse", ARGB))
		RenderMaterial->Diffuse = FColor(ARGB);
		
	if(obj->TryGetNumberField("emissive", ARGB))
		RenderMaterial->Emissive = FColor(ARGB);
	
	return RenderMaterial;
}