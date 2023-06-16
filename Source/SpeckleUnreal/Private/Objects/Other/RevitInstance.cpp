
#include "Objects/Other/RevitInstance.h"
#include "LogSpeckle.h"

#include "API/SpeckleSerializer.h"
#include "Objects/Utils/SpeckleObjectUtils.h"
#include "Transports/Transport.h"

bool URevitInstance::Parse(const TSharedPtr<FJsonObject> Obj,  const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;
	
	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);

	//Transform
	if(!USpeckleObjectUtils::TryParseTransform(Obj, Transform)) return false;
	Transform.ScaleTranslation(FVector(ScaleFactor));
	DynamicProperties.Remove("Transform");
	

	//Geometries
	//NOTE: This logic differs greatly from sharp/py implementations
	const TSharedPtr<FJsonObject>* DefPtr;
	if(!Obj->TryGetObjectField("definition", DefPtr)) return false;
	
	const FString RefID = DefPtr->operator->()->GetStringField("referencedId");
	const TSharedPtr<FJsonObject> Definition = ReadTransport->GetSpeckleObject(RefID);
	
	if(!Obj->TryGetStringField("name", Name))
	{
		if(Definition->TryGetStringField("name", Name))
		{
			//The instance has no name, so we'll steal it from the definition
			DynamicProperties.Add("name", Definition->TryGetField("name"));
		}
	}
	auto Geometries = TArray<TSharedPtr<FJsonValue>>();
	const TArray<TSharedPtr<FJsonValue>>* ElementsPtr;
	if(Definition->TryGetArrayField("elements", ElementsPtr))
		Geometries.Append(*ElementsPtr);
	const TArray<TSharedPtr<FJsonValue>>* DisplayValuePtr;
	if(Definition->TryGetArrayField("displayValue", DisplayValuePtr))
		Geometries.Append(*DisplayValuePtr);
	

	if(Geometries.Num() <= 0)
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Instance definition has no geometry. id: %s"), *RefID)
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
