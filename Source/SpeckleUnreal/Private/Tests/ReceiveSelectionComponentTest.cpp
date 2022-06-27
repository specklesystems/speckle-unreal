// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0


#include "LogSpeckle.h"
#include "Misc/AutomationTest.h"
#include "ReceiveSelectionComponent.h"
#include "Tests/AutomationCommon.h"

#if WITH_DEV_AUTOMATION_TESTS


class UMock : public UReceiveSelectionComponent
{
	friend class FReceiveSelectionComponentTest;
	friend class FCheckValidSelection;
	friend class FCheckLimitMatch;
};

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FCheckValidSelection, FAutomationTestBase*, Test, UMock*, SelectionComponent);
bool FCheckValidSelection::Update()
{
	const UMock* s = SelectionComponent;
	
	Test->TestTrue(TEXT("Account valid on set"), s->IsAccountValid);
	Test->TestTrue(TEXT("Stream valid on set"), s->IsStreamValid);
	Test->TestTrue(TEXT("Branch valid on set"), s->IsBranchValid);
	Test->TestTrue(TEXT("Commit valid on set"), s->IsCommitValid);
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FCheckLimitMatch, FAutomationTestBase*, Test, UMock*, SelectionComponent);
bool FCheckLimitMatch::Update()
{
	const UMock* s = SelectionComponent;
	
	Test->TestTrue(TEXT("Fetched streams is less than limit"), s->Streams.Num() <= s->Limit);
	Test->TestTrue(TEXT("Fetched branches is less than limit"), s->Branches.Num() <= s->Limit);
	Test->TestTrue(TEXT("Fetched commits is less than limit"), s->Commits.Num() <= s->Limit);
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReceiveSelectionComponentTest, "SpeckleUnreal.ReceiveSelectionComponentTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FReceiveSelectionComponentTest::RunTest(const FString& Parameters)
{
#ifdef TEST_AUTH_TOKEN
	UMock* s = NewObject<UMock>();

	
	//Test initialisation
	TestFalse(TEXT("Account valid on initialise"), s->IsAccountValid);
	TestFalse(TEXT("Stream valid on initialise"), s->IsStreamValid);
	TestFalse(TEXT("Branch valid on initialise"), s->IsBranchValid);
	TestFalse(TEXT("Commit valid on initialise"), s->IsCommitValid);
	
	
	s->AuthToken = TEST_AUTH_TOKEN;
	s->ServerUrl = TEXT("https://latest.speckle.dev");
	s->bManualMode = false;
	s->Refresh();
	
	//wait 5 seconds for HTTP requests to finish
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5.0f));
	
	ADD_LATENT_AUTOMATION_COMMAND(FCheckValidSelection(this, s));
	ADD_LATENT_AUTOMATION_COMMAND(FCheckLimitMatch(this, s));

	
#else
	TestTrue(TEXT("TEST_AUTH_TOKEN definition exists"), false);
	
	return false;
#endif
	
	return true;
}
#endif

