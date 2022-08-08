
#include "Objects/Utils/SpeckleObjectUtils.h"

#include "API/SpeckleSerializer.h"
#include "Engine/World.h"
#include "Objects/Geometry/Mesh.h"
#include "Transports/Transport.h"


TArray<TSharedPtr<FJsonValue>> USpeckleObjectUtils::CombineChunks(const TArray<TSharedPtr<FJsonValue>>& ArrayField, const TScriptInterface<ITransport> Transport)
{
	TArray<TSharedPtr<FJsonValue>> ObjectPoints;
		
	for(int32 i = 0; i < ArrayField.Num(); i++) 
	{
		FString Index;
		if(ArrayField[i]->AsObject()->TryGetStringField("referencedId", Index))
		{
			if(!ensureAlwaysMsgf(Transport->HasObject(Index), TEXT("Failed to Dechunk array, Could not find chunk %s in transport"), *Index))
				continue;
			
			const auto Chunk = Transport->GetSpeckleObject(Index)->GetArrayField("data");;
			ObjectPoints.Append(Chunk);
		}
		else
		{
			return ArrayField; //Array was never chunked to begin with
		}
	}
	return ObjectPoints;
}

bool USpeckleObjectUtils::ResolveReference(const TSharedPtr<FJsonObject> Object, const TScriptInterface<ITransport> Transport, TSharedPtr<FJsonObject>& OutObject)
{
	FString SpeckleType;	
	FString ReferenceID;

	if (Object->TryGetStringField("speckle_type", SpeckleType)
		&& SpeckleType == "reference"
		&& Object->TryGetStringField("referencedId",ReferenceID))
	{
		check(Transport != nullptr && Transport.GetObject() != nullptr)
		
		OutObject = Transport->GetSpeckleObject(ReferenceID);
		return OutObject != nullptr;
	}
	return false;
}

float USpeckleObjectUtils::ParseScaleFactor(const FString& UnitsString)
{
	static const auto ParseUnits = [](const FString& LUnits) -> float
	{
		if (LUnits == "millimeters" || LUnits == "millimeter" || LUnits == "millimetres" || LUnits == "millimetre" || LUnits == "mm")
			return 0.1;
		if (LUnits == "centimeters" || LUnits == "centimeter" ||LUnits == "centimetres" || LUnits == "centimetre" || LUnits == "cm")
			return 1;
		if (LUnits == "meters" || LUnits == "meter" || LUnits == "metres" || LUnits == "metre" || LUnits == "m")
			return 100;
		if (LUnits == "kilometers" || LUnits == "kilometres" || LUnits == "km")
			return 100000;

		if (LUnits == "inches" || LUnits == "inch" || LUnits == "in")
			return 2.54;
		if (LUnits == "feet" || LUnits == "foot" || LUnits == "ft")
			return 30.48;
		if (LUnits == "yards" || LUnits == "yard"|| LUnits == "yd")
			return 91.44;
		if (LUnits == "miles" || LUnits == "mile" || LUnits == "mi")
			return 160934.4;

		return 100;
	};

	return ParseUnits(UnitsString.ToLower()); // * WorldToCentimeters; //TODO take into account world units
}

FTransform USpeckleObjectUtils::CreateTransform(UPARAM(ref) const FMatrix& TransformMatrix)
{
	FTransform Transform(TransformMatrix);
	Transform.ScaleTranslation(FVector(1,-1,1));
	FVector Rot = Transform.GetRotation().Euler();
	FVector NewRot(-Rot.X, Rot.Y, -Rot.Z);
	Transform.SetRotation(FQuat::MakeFromEuler(NewRot));
	return Transform;
}

bool USpeckleObjectUtils::TryParseTransform(const TSharedPtr<FJsonObject> SpeckleObject, FMatrix& OutMatrix)
{
	const TSharedPtr<FJsonObject>* TransformObject;
	const TArray<TSharedPtr<FJsonValue>>* TransformData;
		
	if(SpeckleObject->TryGetArrayField("transform", TransformData)) //Handle transform as array
	{ }
	else if(SpeckleObject->TryGetObjectField("transform", TransformObject)
		&& (*TransformObject)->TryGetArrayField("value", TransformData)) //Handle transform as object
	{ }
	else return false;
		
	FMatrix TransformMatrix;
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			TransformMatrix.M[Row][Col] = TransformData->operator[](Row * 4 + Col)->AsNumber();
		}
	OutMatrix = TransformMatrix.GetTransposed();
	return true;
}

bool USpeckleObjectUtils::ParseVectorProperty(const TSharedPtr<FJsonObject> Base, const FString& PropertyName,
	const TScriptInterface<ITransport> ReadTransport, FVector& OutObject)
{
	const TSharedPtr<FJsonObject>* OriginObject;
	if(Base->TryGetObjectField(PropertyName, OriginObject)
		&& ParseVector(*OriginObject, ReadTransport, OutObject))
	{
		return true;
	}
	
	return false;
}

bool USpeckleObjectUtils::ParseVector(const TSharedPtr<FJsonObject> Object,
	const TScriptInterface<ITransport> Transport, FVector& OutObject)
{
	if(!ensure(Object != nullptr)) return false;
	
	TSharedPtr<FJsonObject> Obj;
	if(!ResolveReference(Object, Transport, Obj)) Obj = Object;
	
	double x = 0, y = 0, z = 0;
	
	if(!(Obj->TryGetNumberField("x", x)
		&& Obj->TryGetNumberField("y", y)
		&& Obj->TryGetNumberField("z", z))) return false;

	OutObject = FVector(x,y,z);
	//return true;

	UMesh* Mesh;
	return ParseSpeckleObject<UMesh>(Obj, Transport, Mesh);
}

template <typename TBase>  
bool USpeckleObjectUtils::ParseSpeckleObject(const TSharedPtr<FJsonObject> Object,
	const TScriptInterface<ITransport> Transport, TBase*& OutObject)
{
	static_assert(TIsDerivedFrom<TBase, UBase>::IsDerived, "Type TBase must inherit UBase");

	TSharedPtr<FJsonObject> Obj;
	if(!ResolveReference(Object, Transport, Obj)) Obj = Object;
	
	UBase* b = USpeckleSerializer::DeserializeBase(Object, Transport);
	OutObject = Cast<TBase>(b);
	return OutObject == nullptr;
}


AActor* USpeckleObjectUtils::SpawnActorInWorld(const UObject* WorldContextObject, const TSubclassOf<AActor> Class, UPARAM(ref) const FTransform& Transform)
{
	return WorldContextObject->GetWorld()->SpawnActor(Class, &Transform, FActorSpawnParameters());
}

FString USpeckleObjectUtils::DisplayAsString(const FString& msg, const TSharedPtr<FJsonObject> Obj)
{
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);
	UE_LOG(LogSpeckle, Display, TEXT("resulting jsonString from %s -> %s"), *msg, *OutputString);
	return OutputString;
}
