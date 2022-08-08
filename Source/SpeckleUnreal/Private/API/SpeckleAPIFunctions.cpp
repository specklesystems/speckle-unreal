
#include "API/SpeckleAPIFunctions.h"

#include "JsonObjectConverter.h"

bool USpeckleAPIFunctions::DeserializeResponse(const FString& JsonString, int32& OutStruct)
{
	// We should never hit this!  stubs to avoid NoExport on the class.
	check(0);
	return false;
}


bool USpeckleAPIFunctions::GenericDeserializeResponse(const FString& JsonString, const UScriptStruct* StructType,
	void* OutStruct)
{
	//see FJsonObjectConverter::JsonObjectStringToUStruct
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *JsonString);
		return false;
	}
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructType, OutStruct, 0, 0))
	{
		UE_LOG(LogJson, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *JsonString);
		return false;
	}
	return true;
}

void USpeckleAPIFunctions::CommitReceived(FSpeckleCommit& Commit)
{
	//TODO send read read receipt
	//FString Query = FString::Printf(TEXT("mutation{ commitReceive(input:%s) }"), );
}

DEFINE_FUNCTION(USpeckleAPIFunctions::execDeserializeResponse)
{
	// Get JsonString
	P_GET_PROPERTY(FStrProperty, JsonString);
	
	// Get OutStruct
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* OutBlueprintDataPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	UScriptStruct* StructType = StructProp->Struct;
	
	P_FINISH;
	
	P_NATIVE_BEGIN;
	
	*static_cast<bool*>(RESULT_PARAM) = GenericDeserializeResponse(JsonString, StructType, OutBlueprintDataPtr);
	P_NATIVE_END;
}