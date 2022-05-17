// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#include "Misc/AutomationTest.h"
#include "Objects/Base.h"
#include "Transports/ServerTransport.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FServerTransportTest, "SpeckleUnreal.Transports.ServerTransport", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)


bool FServerTransportTest::RunTest(const FString& Parameters)
{
	FString ServerUrl = TEXT("https://example.com");
	FString StreamId = TEXT("1234");
	FString Token = TEXT("MyAuthToken");
	auto Transport = UServerTransport::CreateServerTransport(ServerUrl, StreamId, Token);
	
	
	return true;
}


#endif
