
#include "Objects/ObjectModelRegistry.h"

#include "Objects/Base.h"
#include "Templates/SubclassOf.h"
#include "UObject/UObjectIterator.h"

TMap<FString, TSubclassOf<UBase>> UObjectModelRegistry::TypeRegistry;

void UObjectModelRegistry::GenerateTypeRegistry()
{
	//TypeRegistry.Reset();
	TypeRegistry.Empty();
	//TypeRegistry = TMap<FString, TSubclassOf<UBase>>();
	//check(TypeRegistry.IsSet());
	
	//Find every class : UBase and add to Registry
	for (TObjectIterator<UClass> It; It; ++It)
	{
		const UClass* Class = *It;
		if (Class->IsChildOf(UBase::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract))
		{
			const FString& SpeckleType = Class->GetDefaultObject<UBase>()->SpeckleType;
			const FString TypeName = GetTypeName(SpeckleType);
			
			ensureAlwaysMsgf(!TypeRegistry.Contains(TypeName),
				TEXT("Base class: %s conflicts with: %s for TypeName: %s"),
				*Class->GetName(),
				*TypeRegistry[SpeckleType]->GetName(),
				*SpeckleType);

			TypeRegistry.Add(TypeName, *It);
		}
	}
}


bool UObjectModelRegistry::SplitSpeckleType(const FString& SpeckleType, FString& OutRemainder, FString& OutTypeName, const FString& Split)
{
	if(SpeckleType.Split(Split, &OutRemainder, &OutTypeName, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
	{
		return true;	
	}
	else
	{
		OutTypeName = SpeckleType;
		return false;
	}
}

FString UObjectModelRegistry::GetTypeName(const FString& SpeckleType)
{
	FString Discard, Ret;
	SplitSpeckleType(SpeckleType, Discard, Ret);
	return Ret;
}

FString UObjectModelRegistry::GetSimplifiedTypeName(const FString& SpeckleType)
{
	FString Discard, Ret;
	SplitSpeckleType(SpeckleType,Discard, Ret, ".");
	return Ret;
}

TSubclassOf<UBase> UObjectModelRegistry::GetAtomicType(const FString& SpeckleType)
{

	FString WorkingType;
	FString Remainder = FString(SpeckleType);

	while(SplitSpeckleType(Remainder, Remainder, WorkingType))
	{
		TSubclassOf<UBase> Type;
		if(TryGetRegisteredType(WorkingType, Type))
		{
			return Type;
		}
	}
	return UBase::StaticClass();
}

TSubclassOf<UBase> UObjectModelRegistry::GetRegisteredType(const FString& TypeName)
{
	TSubclassOf<UBase> Type = nullptr;
	TryGetRegisteredType(TypeName, Type);
	return Type;
}

bool UObjectModelRegistry::TryGetRegisteredType(const FString& TypeName, TSubclassOf<UBase>& OutType)
{
	if(TypeRegistry.Num() == 0) GenerateTypeRegistry();


	const bool Contains = TypeRegistry.Contains(TypeName);
	if(Contains)
	{
		OutType = *TypeRegistry.Find(TypeName);
	}
	return Contains;
}
