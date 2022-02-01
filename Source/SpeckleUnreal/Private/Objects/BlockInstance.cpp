// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockInstance.h"
#include "LogSpeckle.h"

#include "SpeckleUnrealManager.h"

bool UBlockInstance::Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	if(!Super::Parse(Obj, Manager)) return false;
	
	const float ScaleFactor = Manager->ParseScaleFactor(Units);

	//Transform
	const TArray<TSharedPtr<FJsonValue>>* TransformData;
	if(!Obj->TryGetArrayField("transform", TransformData)) return false;
	DynamicProperties.Remove("transform");
	
	FMatrix TransformMatrix;
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			TransformMatrix.M[Row][Col] = TransformData->operator[](Row * 4 + Col)->AsNumber();
		}
	TransformMatrix = TransformMatrix.GetTransposed();
	TransformMatrix.ScaleTranslation(FVector(ScaleFactor));
	Transform = TransformMatrix;

	//Geometries
	const TSharedPtr<FJsonObject>* BlockDefinitionPtr;
	if(!Obj->TryGetObjectField("blockDefinition", BlockDefinitionPtr)) return false;
	
	const FString RefID = BlockDefinitionPtr->operator->()->GetStringField("referencedId");
	const TSharedPtr<FJsonObject> BlockDefinition = Manager->GetSpeckleObject(RefID);
	
	BlockDefinition->TryGetStringField("name", Name);

	const auto Geometries = BlockDefinition->GetArrayField("geometry");

	if(Geometries.Num() <= 0)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Block definition has no geometry. id: %s"), *RefID)
		return false;
	}
	
	for(const auto Geo : Geometries)
	{
		const TSharedPtr<FJsonObject> MeshReference = Geo->AsObject();
		const FString ChildId = MeshReference->GetStringField("referencedId");

		if(Manager->HasObject(ChildId))
		{
			UBase* Child = Manager->DeserializeBase(Manager->GetSpeckleObject(ChildId));
			if(IsValid(Child))
				Geometry.Add(Child);
		}
		else UE_LOG(LogSpeckle, Warning, TEXT("Block definition references an unknown object id: %s"), *ChildId)
		
	}
	
	// Intentionally don't remove blockDefinition from dynamic properties,
	// because we want the converter to create the child geometries for us
	//DynamicProperties.Remove("blockDefinition");
	
	return true;
	
}
