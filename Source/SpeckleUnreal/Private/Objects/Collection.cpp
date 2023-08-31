
#include "Objects/Collection.h"




bool UCollection::Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport)
{
	if(!Super::Parse(Obj, ReadTransport)) return false;

	// Parse Name property
	if(!Obj->TryGetStringField("name", Name)) return false;
	//DynamicProperties.Remove("name"); //Don't remove from dynamic, as we pick this up to name the actor

	// Parse collectionType
	if(!Obj->TryGetStringField("collectionType", CollectionType)) return false;
	DynamicProperties.Remove("collectionType");
	
	return true;
}
