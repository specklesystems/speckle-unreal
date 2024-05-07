
#include "Objects/Other/View3D.h"

#include "Objects/Utils/SpeckleObjectUtils.h"

bool UView3D::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;

	const float ScaleFactor = USpeckleObjectUtils::ParseScaleFactor(Units);

	// Parse optional Name property
	if(Obj->TryGetStringField(TEXT("name"), Name)) { }
	
	// Parse Origin
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, TEXT("origin"), ReadTransport, Origin)) return false;
	Origin *= ScaleFactor;
	DynamicProperties.Remove(TEXT("origin"));

	// Parse UpDirection
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, "upDirection", ReadTransport, UpDirection)) return false;
	UpDirection *= ScaleFactor;
	DynamicProperties.Remove(TEXT("upDirection"));
	
	// Parse ForwardDirection
	if(!USpeckleObjectUtils::ParseVectorProperty(Obj, TEXT("forwardDirection"), ReadTransport, ForwardDirection)) return false;
	ForwardDirection *= ScaleFactor;
	DynamicProperties.Remove(TEXT("forwardDirection"));

	// Parse IsOrthogonal
	if(!Obj->TryGetBoolField(TEXT("isOrthogonal"), IsOrthogonal)) return false;
	DynamicProperties.Remove(TEXT("isOrthogonal"));
	
	return true;
}
