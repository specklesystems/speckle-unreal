
#include "ReceiveSelectionComponent.h"

#include "JsonObjectConverter.h"
#include "LogSpeckle.h"
#include "API/ClientAPI.h"
#include "API/Models/SpeckleUser.h"

#if WITH_EDITOR
void UReceiveSelectionComponent::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	const FName PropertyName = (e.Property != nullptr) ? e.Property->GetFName() : NAME_None;
	PropertyChangeHandler(PropertyName);
}
#endif

void UReceiveSelectionComponent::PropertyChangeHandler(const FName& PropertyName)
{
	if(bManualMode) return;
		
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, bManualMode))
	{
		Refresh();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, AuthToken)
		|| PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, ServerUrl))
	{
		IsAccountValid = !AuthToken.IsEmpty() && !ServerUrl.IsEmpty();
		if(IsAccountValid)
		{
			//TODO maybe we should check URL is a valid URL?
			ServerUrl.TrimEndInline();
			while(ServerUrl.RemoveFromEnd("/")) { }
			AuthToken.TrimEndInline();
		}

		UpdateStreams();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, SelectedStreamText)
		&& !SelectedStreamText.IsEmpty())
	{
		IsStreamValid = Streams.Contains(SelectedStreamText);
		if(IsStreamValid)
		{
			SelectStream(SelectedStreamText);
		}
		else UpdateStreams();
	} 
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, SelectedBranchText)
		&& !SelectedBranchText.IsEmpty())
	{
		IsBranchValid = Branches.Contains(SelectedBranchText);
		if(IsBranchValid)
		{
			SelectBranch(SelectedBranchText);
		}
		else UpdateBranches();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UReceiveSelectionComponent, SelectedCommitText)
		&& !SelectedCommitText.IsEmpty())
	{
		IsCommitValid = Commits.Contains(SelectedCommitText);
		if(IsCommitValid)
		{
			SelectCommit(SelectedCommitText);
		}
		else UpdateCommits();
	}
}


void UReceiveSelectionComponent::Refresh()
{
	IsAccountValid = !AuthToken.IsEmpty() && !ServerUrl.IsEmpty();

	UpdateStreams();
}


bool UReceiveSelectionComponent::TryGetSelectedCommit(FSpeckleCommit& OutCommit) const
{
	if(!bManualMode && IsCommitValid)
	{
		OutCommit = Commits.FindRef(SelectedCommitText);
		return true;
	}
	return false;
}

bool UReceiveSelectionComponent::TryGetSelectedBranch(FSpeckleBranch& OutBranch) const
{
	if(!bManualMode && IsBranchValid)
	{
		OutBranch = Branches.FindRef(SelectedBranchText);
		return true;
	}
	return false;
}

bool UReceiveSelectionComponent::TryGetSelectedStream(FSpeckleStream& OutStream) const
{
	if(!bManualMode && IsStreamValid)
	{
		OutStream = Streams.FindRef(SelectedStreamText);
		return true;
	}
	return false;
}

void UReceiveSelectionComponent::OpenURLInBrowser() const
{
	const FString URL = GetUrl();

	if(!ensure(FPlatformProcess::CanLaunchURL(*URL))) return;

	FString LaunchErrors;
	FPlatformProcess::LaunchURL(*URL, nullptr, &LaunchErrors);

	if(LaunchErrors.IsEmpty())
	{
		UE_LOG(LogSpeckle, Log, TEXT("Launched URL \"%s\" in browser"), *URL);
	}
	else
	{
		UE_LOG(LogSpeckle, Warning, TEXT("Failed to launched URL \"%s\" in browser - %s"), *URL, *LaunchErrors);
	}

}

bool UReceiveSelectionComponent::IsSelectionComplete(FString& OutStatusMessage) const
{
	if(bManualMode)
	{
		if(ServerUrl.IsEmpty()) OutStatusMessage = TEXT("Server Url is invalid, must specify a valid url");
		else if(StreamId.IsEmpty()) OutStatusMessage = TEXT("Stream id is invalid, must specify a streamId");
		else if(ObjectId.IsEmpty()) OutStatusMessage = TEXT("Object id is invalid, must specify a objectId");
		else
		{
			OutStatusMessage = TEXT("Ready to recieve object");
			return true;
		}
	}
	else
	{
		if(!IsAccountValid) OutStatusMessage = TEXT("Account is invalid, please specify a valid server URL and token. (tokens can be created from your Speckle profiles page)"); 
		else if(!IsStreamValid) OutStatusMessage = TEXT("Selected stream is invalid");
		else if(!IsBranchValid) OutStatusMessage = TEXT("Selected branch is invalid");
		else if(!IsCommitValid) OutStatusMessage = TEXT("Selected commit is invalid");
		else
		{
			ensure(!ServerUrl.IsEmpty());
			ensure(!StreamId.IsEmpty());
			ensure(!ObjectId.IsEmpty());
			OutStatusMessage = TEXT("Ready to recieve commit");
			return true;
		}
	}
	return false;
}

FString UReceiveSelectionComponent::GetUrl() const
{
	if(bManualMode)
	{
		return FString::Printf(TEXT("%s/streams/%s/objects/%s"),
			*ServerUrl, *StreamId, *ObjectId);
	}
	
	if(!IsStreamValid) return ServerUrl;
	
	if(!IsBranchValid)
		return FString::Printf(TEXT("%s/streams/%s/"),
			*ServerUrl, *GetSelectedStream().ID);

	if(!IsCommitValid)
		return FString::Printf(TEXT("%s/streams/%s/branches/%s"),
			*ServerUrl, *GetSelectedStream().ID, *GetSelectedBranch().Name);
	
	return FString::Printf(TEXT("%s/streams/%s/commits/%s"),
			*ServerUrl, *GetSelectedStream().ID, *GetSelectedCommit().ID);
}

TArray<FString> UReceiveSelectionComponent::GetStreamsOptions() const
{
	TArray<FString> Options;
	Streams.GetKeys(Options);
	return Options;
}

bool UReceiveSelectionComponent::SelectStream(const FString& DisplayId)
{
	const bool IsValid = !DisplayId.IsEmpty() && Streams.Contains(DisplayId);
	if(IsValid && !bManualMode)
	{
		SelectedStreamText = DisplayId;
		IsStreamValid = true;
		StreamId = GetSelectedStream().ID;
	}
	else
	{
		SelectedStreamText = "";
		IsStreamValid = false;
	}
	UpdateBranches();
	return IsStreamValid;
}

void UReceiveSelectionComponent::UpdateStreams()
{
	Streams.Reset();
	SelectStream("");

	if(!IsAccountValid || bManualMode) return;
	
	const FString LogName(__FUNCTION__);
	const FString Payload = FString::Printf(TEXT("{\"query\": \"query{user{id streams(limit: %d){items{id name}}}}\"}"), Limit);

	//Response Handling
	auto OnComplete = [&](const FString& ResponseJson)
	{
		if(bManualMode) return;

		FSpeckleUser Response;

		if(!ensure(FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseJson, &Response, 0, 0))) return;

		const TArray<FSpeckleStream>& AvailableStreams = Response.Streams.Items;
		//Assemble stream map
		Streams.Reserve(AvailableStreams.Num());
		for(const auto& s : Response.Streams.Items)
		{
			FString DisplayId = FString::Printf(TEXT("%s - %s"), *s.Name, *s.ID);
			Streams.Add(DisplayId, s);
		}

		//Set default selection
		if(AvailableStreams.Num() > 0)
		{
			const FSpeckleStream& s = AvailableStreams[0];
			FString DisplayId = FString::Printf(TEXT("%s - %s"), *s.Name, *s.ID);

			SelectStream(DisplayId);
		}
		
		UE_LOG(LogSpeckle, Verbose, TEXT("%s was successful"), *LogName);
	};
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindLambda(OnComplete);

	//On error
	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindStatic(LogError, LogName);

	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "user", Payload, LogName, CompleteDelegate, ErrorDelegate);
	
}

FSpeckleStream UReceiveSelectionComponent::GetSelectedStream() const
{
	return Streams[SelectedStreamText];
}

TArray<FString> UReceiveSelectionComponent::GetBranchOptions()
{
	TArray<FString> Options;
	Branches.GetKeys(Options);
	return Options;
}

bool UReceiveSelectionComponent::SelectBranch(const FString& DisplayId)
{
	const bool IsValid = !DisplayId.IsEmpty() && Branches.Contains(DisplayId);
	if(IsValid && !bManualMode)
	{
		SelectedBranchText = DisplayId;
		IsBranchValid = true;
	}
	else
	{
		SelectedBranchText = "";
		IsBranchValid=false;
	}
	UpdateCommits();
	return IsBranchValid;
}


void UReceiveSelectionComponent::UpdateBranches()
{
	Branches.Reset();
	SelectBranch("");

	if(!IsStreamValid || bManualMode) return;

	const FString LogName(__FUNCTION__);
	const FString Payload = FString::Printf(TEXT("{\"query\": \"query{stream(id: \\\"%s\\\"){branches(limit: %d){items{id name}}}}\"}"), *GetSelectedStream().ID, Limit);

	//Response Handling
	auto OnComplete = [&](const FString& ResponseJson)
	{
		if(bManualMode) return;
		
		FSpeckleStream Response;
		
		if(!ensure(FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseJson, &Response, 0, 0))) return;

		const TArray<FSpeckleBranch>& AvailableBranches = Response.Branches.Items;
		
		//Assemble stream map
		Branches.Reserve(AvailableBranches.Num());
		for(const auto& b : AvailableBranches)
		{
			FString DisplayId = FString::Printf(TEXT("%s"), *b.Name);
			Branches.Add(DisplayId, b);
		}
		
		UE_LOG(LogSpeckle, Verbose, TEXT("%s was successful"), *LogName);
		
		//Set default selection
		if(AvailableBranches.Num() > 0)
		{
			const FSpeckleBranch& b = AvailableBranches[0];
			FString DisplayId = FString::Printf(TEXT("%s"), *b.Name);

			SelectBranch(DisplayId);
		}
	};
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindLambda(OnComplete);

	//On error
	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindStatic(LogError, LogName);

	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "stream", Payload,LogName, CompleteDelegate, ErrorDelegate);
}


FSpeckleBranch UReceiveSelectionComponent::GetSelectedBranch() const
{
	return Branches[SelectedBranchText];
}

TArray<FString> UReceiveSelectionComponent::GetCommitOptions()
{
	TArray<FString> Options;
	Commits.GetKeys(Options);
	return Options;
}

bool UReceiveSelectionComponent::SelectCommit(const FString& DisplayId)
{
	const bool IsValid = !DisplayId.IsEmpty() && Commits.Contains(DisplayId);
	if(IsValid && !bManualMode)
	{
		SelectedCommitText = DisplayId;
		IsCommitValid = true;
		ObjectId = GetSelectedCommit().ReferencedObject;
	}
	else
	{
		SelectedCommitText = "";
		IsCommitValid=false;
		ObjectId = "";
	}
	return IsCommitValid;
}

void UReceiveSelectionComponent::UpdateCommits()
{
	Commits.Reset();
	SelectCommit("");

	if(!IsBranchValid || bManualMode) return;
	
	const FString LogName(__FUNCTION__);
	const FString Payload = FString::Printf(TEXT("{\"query\": \"query{stream(id: \\\"%s\\\"){branch(name: \\\"%s\\\"){commits(limit: %d){items{id message referencedObject}}}}}\"}"), *GetSelectedStream().ID, *GetSelectedBranch().Name, Limit);
	
	//Response Handling
	auto OnComplete = [&](const FString& ResponseJson)
	{
		if(bManualMode) return;
		
		FSpeckleStream Response;

		if(!ensure(FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseJson, &Response, 0, 0))) return;

		const TArray<FSpeckleCommit>& AvailableCommits = Response.Branch.Commits.Items;
		
		//Assemble stream map
		for(const auto& c :AvailableCommits)
		{
			FString DisplayId = FString::Printf(TEXT("%s - %s"), *c.Message, *c.ID);
			Commits.Add(DisplayId, c);
		}
		
		UE_LOG(LogSpeckle, Verbose, TEXT("%s was successful"), *LogName);
		
		//Set default selection
		if(AvailableCommits.Num() > 0)
		{
			const FSpeckleCommit& c = AvailableCommits[0];
			FString DisplayId = FString::Printf(TEXT("%s - %s"), *c.Message, *c.ID);
			SelectCommit(DisplayId);
		}
	};
	FAPIResponceDelegate CompleteDelegate;
	CompleteDelegate.BindLambda(OnComplete);

	//On error
	FErrorDelegate ErrorDelegate;
	ErrorDelegate.BindStatic(LogError, LogName);

	FClientAPI::MakeGraphQLRequest(ServerUrl, AuthToken, "stream", Payload,LogName, CompleteDelegate, ErrorDelegate);
}

FSpeckleCommit UReceiveSelectionComponent::GetSelectedCommit() const
{
	return Commits[SelectedCommitText];
}

void UReceiveSelectionComponent::LogError(const FString& Message, const FString LogName)
{
	UE_LOG(LogSpeckle, Warning, TEXT("%s: %s"), *LogName, *Message)
}
