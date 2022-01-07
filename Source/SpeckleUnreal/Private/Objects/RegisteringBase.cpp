#include "Objects/RegisteringBase.h"

#include "Objects/Base.h"

TOptional<TMap<FString, TSubclassOf<UBase>>> URegisteringBase::TypeRegistry;

void URegisteringBase::GenerateTypeRegistry()
{
	TypeRegistry.Reset();
	TypeRegistry = TMap<FString, TSubclassOf<UBase>>();
	check(TypeRegistry.IsSet());
	
	//Find every class : UBase and add to Registry
	for (TObjectIterator<UClass> It; It; ++It)
	{
		const UClass* Class = *It;
		if (Class->IsChildOf(UBase::StaticClass()) &&
			!Class->HasAnyClassFlags(CLASS_Abstract))
		{
			const FString& SpeckleType = Class->GetDefaultObject<UBase>()->SpeckleType;;

			ensureAlwaysMsgf(!TypeRegistry->Contains(SpeckleType),
				TEXT("Base class: %s conflicts with: %s for SpeckleType: %s"),
				*Class->GetName(),
				*TypeRegistry->operator[](SpeckleType)->GetName(),
				*SpeckleType);

			TypeRegistry->Add(SpeckleType, *It);
		}
	}
}

TSubclassOf<UBase> URegisteringBase::GetRegisteredType(const FString& SpeckleType)
{
	if(!TypeRegistry.IsSet()) GenerateTypeRegistry();
		
	return TypeRegistry->Find(SpeckleType)->Get();
}