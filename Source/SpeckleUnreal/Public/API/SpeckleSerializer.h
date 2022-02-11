#pragma once

class UBase;
class ITransport;

class FSpeckleSerializer
{
public:
	static UBase* DeserializeBase(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport);
	
};
