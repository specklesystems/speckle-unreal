#include "API/Operations/SendOperation.h"

#include "Mixpanel.h"
#include "Transports/Transport.h"


USendOperation* USendOperation::SendOperation(UObject* WorldContextObject, UBase* Base, TArray<TScriptInterface<ITransport>> Transports)
{
	
	USendOperation* Node = NewObject<USendOperation>();
	Node->Base = Base;
	Node->Transports = Transports;

	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void USendOperation::Activate()
{
	FAnalytics::TrackEvent("unknown", "unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});
	Send();
}

void USendOperation::Send()
{
	check(Transports.Num() > 0);

	for (const auto t : Transports)
	{
		//TODO
		//ITransport::
	}
}