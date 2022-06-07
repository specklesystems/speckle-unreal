// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Geometry/Box.h"

#include "Objects/Geometry/Plane.h"
#include "Objects/Utils/SpeckleObjectUtils.h"

bool UBoxx::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;
	
	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);

	const FString BasePlanePropertyName = TEXT("basePlane");
	if (Obj->HasField(BasePlanePropertyName))
	{
		BasePlane = NewObject<UPlane>();
		if(!BasePlane->Parse(Obj->GetObjectField(BasePlanePropertyName), ReadTransport)) return false;
		DynamicProperties.Remove(BasePlanePropertyName);
	}

	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "xSize", ReadTransport, XSize)) return false;
	XSize *= ScaleFactor;
	DynamicProperties.Remove("xSize");
	
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "ySize", ReadTransport, YSize)) return false;
	YSize *= ScaleFactor;
	DynamicProperties.Remove("ySize");
	
	
	return true;
}

UMesh* UBoxx::ToMesh(UObject* Outer) const
{

	TArray<FVector> Vertices = {
		FVector(),
		FVector(),
		FVector(),
		FVector(),
		FVector(),
		FVector(),
	};

	TArray<int32> Faces = {
	};
	
	//FMatrix transform = 
	
	
	UMesh* Mesh = NewObject<UMesh>(Outer);
	return Mesh;
}
