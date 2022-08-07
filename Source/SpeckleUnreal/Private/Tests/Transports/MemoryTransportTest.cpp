
#include "Misc/AutomationTest.h"
#include "Objects/Base.h"
#include "Transports/MemoryTransport.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMemoryTransportTest, "SpeckleUnreal.Transports.MemoryTransport", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)


bool FMemoryTransportTest::RunTest(const FString& Parameters)
{
	UMemoryTransport* Transport = UMemoryTransport::CreateEmptyMemoryTransport();
	// Test Construction
	{
		TestNotNull(TEXT("Constructed object"), Transport);
		TestTrue(TEXT("Constructed object is valid"), IsValid(Transport));
	}
	
	TSharedPtr<FJsonObject> MockObj = MakeShareable(new FJsonObject());
	FString TestId = TEXT("testidlalalala");
	FString TestPayloadName = TEXT("Playload");
	FString TestPayload = TEXT("MyPayloadValue!");
	MockObj->SetStringField(TestPayloadName, TestPayload);
	
	// Test Save
	{
		Transport->SaveObject(TestId, MockObj);
		TestTrue(TEXT("Transport with save object to HasObject"), Transport->HasObject(TestId));
		
		auto Value = Transport->GetSpeckleObject(TestId);
		TestNotNull(TEXT("Return of getting saved object"), Value.Get());
		TestEqual(TEXT("Return of getting saved object"), Value, MockObj);
		TestTrue(TEXT("Returned object to have payload"), Value->HasField(TestPayloadName));
		TestEqual(TEXT("Returned object's playload"), Value->GetStringField(TestPayloadName), TestPayload);
	}

	// Test Ids are missing
	{
		TArray<FString> MissingIds = {
			TEXT("NoObjectsWithThisId"),
			TEXT("testidla"),
			TEXT("testidlalalalaextrala"),
			TEXT("testidrararara"),
			TEXT(""),
		};
		
		for(const FString& Id : MissingIds)
		{
			TestFalse(TEXT("transport has unknown id -") + Id, Transport->HasObject(Id));
			auto ValueM = Transport->GetSpeckleObject(Id);
			TestNull(TEXT("return of unknown id -") + Id, ValueM.Get());
		}
	}
	
	return true;
}


#endif
