// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Geometry/Plane.h"

#include "Objects/Utils/SpeckleObjectUtils.h"

bool UPlane::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;
	
	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);
	
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "origin", ReadTransport, Origin)) return false;
	Origin *= ScaleFactor;
	DynamicProperties.Remove("origin");
	
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "normal", ReadTransport, Normal)) return false;
	Normal *= ScaleFactor;
	DynamicProperties.Remove("normal");
	
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "ydir", ReadTransport, XDir)) return false;
	XDir *= ScaleFactor;
	DynamicProperties.Remove("ydir");
	
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "xdir", ReadTransport, YDir)) return false;
	YDir *= ScaleFactor;
	DynamicProperties.Remove("xdir");
		
	return true;
}
