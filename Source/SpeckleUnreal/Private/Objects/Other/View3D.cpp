
#include "Objects/Other/View3D.h"

#include "Objects/Utils/SpeckleObjectUtils.h"

bool UView3D::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;

	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);

	// Parse optional Name property
	if(Obj->TryGetStringField("name", Name)) { }
	
	// Parse Origin
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "origin", ReadTransport, Origin)) return false;
	Origin *= ScaleFactor;
	DynamicProperties.Remove("origin");

	// Parse UpDirection
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "upDirection", ReadTransport, UpDirection)) return false;
	UpDirection *= ScaleFactor;
	DynamicProperties.Remove("upDirection");
	
	// Parse ForwardDirection
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "forwardDirection", ReadTransport, ForwardDirection)) return false;
	ForwardDirection *= ScaleFactor;
	DynamicProperties.Remove("forwardDirection");

	// Parse IsOrthogonal
	if(!Obj->TryGetBoolField("isOrthogonal", IsOrthogonal)) return false;
	DynamicProperties.Remove("isOrthogonal");
	
	return true;
}
