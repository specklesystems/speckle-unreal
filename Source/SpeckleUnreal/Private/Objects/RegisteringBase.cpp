#include "Objects/RegisteringBase.h"

#include "Objects/Base.h"

//TOptional<TMap<FString, TSubclassOf<UBase>>> URegisteringBase::TypeRegistry;
TMap<FString, TSubclassOf<UBase>> URegisteringBase::TypeRegistry;

void URegisteringBase::GenerateTypeRegistry()
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
			const FString& SpeckleType = Class->GetDefaultObject<UBase>()->SpeckleType;;

			ensureAlwaysMsgf(!TypeRegistry.Contains(SpeckleType),
				TEXT("Base class: %s conflicts with: %s for SpeckleType: %s"),
				*Class->GetName(),
				*TypeRegistry[SpeckleType]->GetName(),
				*SpeckleType);

			TypeRegistry.Add(SpeckleType, *It);
		}
	}
}



TSubclassOf<UBase> URegisteringBase::FindClosestType(const FString& SpeckleType)
{
	FString TypeString(SpeckleType);
	TSubclassOf<UBase> Type = nullptr;

	while(!TryGetRegisteredType(TypeString, Type))
	{
		int32 SplitIndex;
		if(TypeString.FindLastChar('.', SplitIndex))
		{
			TypeString = TypeString.Left(SplitIndex);
		}
		else return nullptr;
	}
		
	return Type;
	
}

TSubclassOf<UBase> URegisteringBase::GetRegisteredType(const FString& SpeckleType)
{
	TSubclassOf<UBase> Type = nullptr;
	TryGetRegisteredType(SpeckleType, Type);
	return Type;
}

bool URegisteringBase::TryGetRegisteredType(const FString& SpeckleType, TSubclassOf<UBase>& OutType)
{
	if(TypeRegistry.Num() == 0) GenerateTypeRegistry();


	const bool Contains = TypeRegistry.Contains(SpeckleType);
	if(Contains)
	{
		OutType = *TypeRegistry.Find(SpeckleType);
	}
	return Contains;
}
