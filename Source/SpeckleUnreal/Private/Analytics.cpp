
#include "Mixpanel.h"

#include "Containers/UnrealString.h"
#include "HttpModule.h"
#include "Kismet/GameplayStatics.h"
#include "LogSpeckle.h"
#include "Interfaces/IHttpResponse.h"
#include "Launch/Resources/Version.h"
#include "Misc/Base64.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Serialization/JsonWriter.h"

const FString FAnalytics::MixpanelToken = TEXT("acd87c5a50b56df91a795e999812a3a4");
const FString FAnalytics::MixpanelServer = TEXT("https://analytics.speckle.systems");
const FString FAnalytics::VersionedApplicationName = FString::Printf(TEXT("Unreal Engine %d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);

void FAnalytics::TrackEvent(const FString& Server, const FString& EventName)
{
	const TMap<FString, FString> CustomProperties;
	TrackEvent( Server, EventName, CustomProperties);
}

void FAnalytics::TrackEvent(const FString& Server, const FString& EventName, const TMap<FString, FString>& CustomProperties)
{
	//Since we don't have access to users email address, we will hash the system user name instead (better than not tracking users at all) 
	const FString UserID = FString(FPlatformProcess::ComputerName()) + FString(FPlatformProcess::UserName());
	TrackEvent(UserID, Server, EventName, CustomProperties);
}

void FAnalytics::TrackEvent(const FString& UserID, const FString& Server, const FString& EventName, const TMap<FString, FString>& CustomProperties)
{

	FString HashedUserID = "@" + Hash(UserID); //prepending an @ so we distinguish logged and non-logged users
	FString HashedServer = Hash(Server);
	
	TMap<FString, FString> Properties 
	{
		{ "distinct_id", HashedUserID },
		{ "server_id", HashedServer },
		{ "token", MixpanelToken },
		{ "hostApp", "Unreal Engine" },
		{ "hostAppVersion", VersionedApplicationName },
		{ "$os",  *UGameplayStatics::GetPlatformName() },
		{ "type", "action" }
	};

	
	Properties.Append(CustomProperties);
	
	FString Json;
	{
		auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Json);
		FJsonSerializerWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> Serializer(Writer);
		Serializer.StartObject();
		FString EventNameString = EventName;
		Serializer.Serialize(TEXT("event"), EventNameString);
		Serializer.SerializeMap(TEXT("properties"), Properties);
		Serializer.EndObject();
		Writer->Close();
	}
	const FString Data = FBase64::Encode(Json);

	// Create Request
	const FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	{
		Request->SetURL(MixpanelServer + "/track?ip=1&data=" + Data);
		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
		Request->SetHeader("Accept", TEXT("text/plain"));
		Request->SetVerb("POST");
	}

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool)
	{
		UE_LOG(LogSpeckle, Verbose, TEXT("Recieved Mixpanel resonse %d"), Response->GetResponseCode());
	});

	const bool RequestSent = Request->ProcessRequest();
	if(!RequestSent)
	{
		UE_LOG(LogSpeckle, Log, TEXT("Failed to send Mixpanel event to %s"), *MixpanelServer);
	}
}

FString FAnalytics::Hash(const FString& Input)
{
	return FMD5::HashAnsiString(*Input.ToLower());
}
