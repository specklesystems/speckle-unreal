
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



TSubclassOf<UBase> UObjectModelRegistry::FindClosestType(const FString& SpeckleType)
{
	FString TypeString(SpeckleType);
	TSubclassOf<UBase> Type = nullptr;

	while(!TryGetRegisteredType(TypeString, Type))
	{
		if(!ParentType(TypeString, TypeString)) return nullptr;
	}
		
	return Type;
	
}

bool UObjectModelRegistry::ParentType(const FString& Type, FString& NextType)
{
	int32 DotSplitIndex;
	Type.FindLastChar('.', DotSplitIndex);
	int32 ColonSplitIndex;
	Type.FindLastChar(':', ColonSplitIndex);
	const int32 SplitIndex = FGenericPlatformMath::Max(DotSplitIndex, ColonSplitIndex);

	if(SplitIndex <= 0) return false;
		
	NextType = Type.Left(SplitIndex);
	return true;
}

TSubclassOf<UBase> UObjectModelRegistry::GetRegisteredType(const FString& SpeckleType)
{
	TSubclassOf<UBase> Type = nullptr;
	TryGetRegisteredType(SpeckleType, Type);
	return Type;
}

bool UObjectModelRegistry::TryGetRegisteredType(const FString& SpeckleType, TSubclassOf<UBase>& OutType)
{
	if(TypeRegistry.Num() == 0) GenerateTypeRegistry();


	const bool Contains = TypeRegistry.Contains(SpeckleType);
	if(Contains)
	{
		OutType = *TypeRegistry.Find(SpeckleType);
	}
	return Contains;
}
