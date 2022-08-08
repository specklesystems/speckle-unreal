
#include "Objects/Other/BlockInstance.h"
#include "LogSpeckle.h"

#include "API/SpeckleSerializer.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Transports/Transport.h"

bool UBlockInstance::Parse(const TSharedPtr<FJsonObject> Obj,  const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;
	
	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);

	//Transform
	if(!USpeckleObjectUtils::TryParseTransform(Obj, Transform)) return false;
	Transform.ScaleTranslation(FVector(ScaleFactor));
	DynamicProperties.Remove("Transform");
	

	//Geometries
	const TSharedPtr<FJsonObject>* BlockDefinitionPtr;
	if(!Obj->TryGetObjectField("blockDefinition", BlockDefinitionPtr)) return false;
	
	const FString RefID = BlockDefinitionPtr->operator->()->GetStringField("referencedId");
	const TSharedPtr<FJsonObject> BlockDefinition = ReadTransport->GetSpeckleObject(RefID);
	
	if(BlockDefinition->TryGetStringField("name", Name)) DynamicProperties.Remove("Name");

	const auto Geometries = BlockDefinition->GetArrayField("geometry");

	if(Geometries.Num() <= 0)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Block definition has no geometry. id: %s"), *RefID)
		return false;
	}
	
	for(const auto& Geo : Geometries)
	{
		const TSharedPtr<FJsonObject> MeshReference = Geo->AsObject();
		const FString ChildId = MeshReference->GetStringField("referencedId");

		if(ReadTransport->HasObject(ChildId))
		{
			UBase* Child = USpeckleSerializer::DeserializeBase(ReadTransport->GetSpeckleObject(ChildId), ReadTransport);
			if(IsValid(Child))
				Geometry.Add(Child);
		}
		else UE_LOG(LogSpeckle, Warning, TEXT("Block definition references an unknown object id: %s"), *ChildId)
	}
	DynamicProperties.Remove("geometry");
	
	// Intentionally don't remove blockDefinition from dynamic properties,
	// because we want the converter to create the child geometries for us
	//DynamicProperties.Remove("blockDefinition");
	
	return true;
	
}
