// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"

///  Anonymous telemetry to help us understand how to make a better Speckle.
///  This really helps us to deliver a better open source project and product!
class FAnalytics
{
protected:
	static const FString MixpanelToken;
	static const FString MixpanelServer;
	static const FString VersionedApplicationName;

public:
	static void TrackEvent(const FString& Server, const FString& EventName);
	static void TrackEvent(const FString& Server, const FString& EventName, const TMap<FString, FString>& CustomProperties);
	static void TrackEvent(const FString& Email, const FString& Server, const FString& EventName, const TMap<FString, FString>& CustomProperties);
	static FString Hash(const FString& Input);
};