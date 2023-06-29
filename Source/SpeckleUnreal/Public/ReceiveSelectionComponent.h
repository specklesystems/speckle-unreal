
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "API/Models/SpeckleStream.h"

#include "ReceiveSelectionComponent.generated.h"


/**
 * Actor component for selecting a Speckle Stream + Object
 * Either by Stream Id + Object Id (Object selection mode)
 * Or by Stream + Branch + Commit (Commit selection mode)
 */
UCLASS(ClassGroup=(Speckle), meta=(BlueprintSpawnableComponent))
class SPECKLEUNREAL_API UReceiveSelectionComponent : public UActorComponent
{
    GENERATED_BODY()
	
public:

    // URL of the speckle server 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle")
	FString ServerUrl = "https://speckle.xyz";
	
	// A Personal Access Token can be created from your Speckle Profile page (Treat tokens like passwords, do not share publicly)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", meta=(PasswordField=true))
	FString AuthToken;
	
	// Id of the stream to receive from (checkout the stream URL in the Speckle web viewer)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", meta=(EditCondition="bManualMode", EditConditionHides, DisplayAfter=bManualMode))
	FString StreamId;

	// Id of the Speckle Object to receive (checkout the properties panel in the Speckle web viewer)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", meta=(EditCondition="bManualMode", EditConditionHides, DisplayAfter=StreamId))
	FString ObjectId;
	
	/**
	 * Trys to get the currently selected Commit (Commit selection mode)
	 * @param OutCommit the selected Commit (if return was true)
	 * @returns true if a selection was made
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle")
	bool TryGetSelectedCommit(FSpeckleCommit& OutCommit) const;

	/**
	 * Trys to get the currently selected Branch (Commit selection mode)
     * @param OutBranch the selected Branch (if return was true)
     * @returns true if a selection was made
     */
	UFUNCTION(BlueprintCallable, Category="Speckle")
	bool TryGetSelectedBranch(FSpeckleBranch& OutBranch) const;

	/**
	 * Trys to get the currently selected Stream (Commit selection mode)
	 * @param OutStream the selected Stream (if return was true)
	 * @returns true if a selection was made
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle")
	bool TryGetSelectedStream(FSpeckleStream& OutStream) const;
	
	/**
	 * @param OutStatusMessage the current status of the object selection as a human readable string
	 * @returns true if a complete object selection is made.
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle", CallInEditor)
    bool IsSelectionComplete(FString& OutStatusMessage) const;
	
	// Gets the URL of the current selection/partial selection
	UFUNCTION(BlueprintCallable, Category="Speckle", CallInEditor)
    FString GetUrl() const;
	
	/**
	 * Launches the URL of the current selection/partial selection using FPlatformProcess
	 * @returns true if the URL could be launched
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle", CallInEditor)
    void OpenURLInBrowser() const;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PropertyChangeHandler(const FName& PropertyName);
	
protected: //Internal logic for branch/stream/commit fetching and selection
	
	// When true, user specifies stream id + object id, when false, user specifies stream + branch + commit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="Speckle", meta=(DisplayName="Specify by Object Id", DisplayAfter=AuthToken));
	bool bManualMode = true;
	
	// Limit of how many streams/branches/commits to fetch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Speckle", AdvancedDisplay, meta=(DisplayName="Options Limit", ClampMin=0, ClampMax=100, UIMin = 1, UIMax=30))
	int32 Limit = 15;
	
	// Refreshes the Stream/Branch/Commit options
	UFUNCTION(BlueprintCallable, Category="Speckle", CallInEditor)
	void Refresh();

#pragma region Stream	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="Speckle", meta=(DisplayName="Stream", GetOptions=GetStreamsOptions, EditCondition="IsAccountValid && !bManualMode", NoResetToDefault))
	FString SelectedStreamText;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	TMap<FString, FSpeckleStream> Streams;

    UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual TArray<FString> GetStreamsOptions() const;
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
    bool SelectStream(const FString& DisplayId);

    UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual void UpdateStreams();
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual FSpeckleStream GetSelectedStream() const;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	bool IsStreamValid = false;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	bool IsAccountValid = false;

#pragma endregion

#pragma region Branch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="Speckle", meta=(DisplayName="Branch", GetOptions=GetBranchOptions, EditCondition="IsStreamValid && !bManualMode", NoResetToDefault))
	FString SelectedBranchText;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	TMap<FString, FSpeckleBranch> Branches;
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual TArray<FString> GetBranchOptions();
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
    bool SelectBranch(const FString& DisplayId);

    UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual void UpdateBranches();

	UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual FSpeckleBranch GetSelectedBranch() const;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	bool IsBranchValid = false;

#pragma endregion

#pragma region Commit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, Category="Speckle", meta=(DisplayName="Commit", GetOptions=GetCommitOptions, EditCondition="IsCommitValid && !bManualMode", NoResetToDefault))
	FString SelectedCommitText;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	TMap<FString, FSpeckleCommit> Commits;
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual TArray<FString> GetCommitOptions();
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
    bool SelectCommit(const FString& DisplayId);

    UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual void UpdateCommits();
	
	UFUNCTION(BlueprintCallable, Category="Speckle")
	virtual FSpeckleCommit GetSelectedCommit() const;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Speckle")
	bool IsCommitValid = false;

#pragma endregion

    static void LogError(const FString& Message, const FString LogName);
};

