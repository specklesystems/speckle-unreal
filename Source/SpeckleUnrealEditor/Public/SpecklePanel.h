#pragma once

#include "SpeckleUnreal/Public/Panagiotis/SpeckleStructs.h"
#include "SpeckleUnreal/Public/SpeckleUnrealManager.h"
#include "SpeckleUnreal/Public/Panagiotis/SpeckleRESTHandlerComponent.h"

struct FSpeckleBranch;
struct FSpeckleCommit;

class SpecklePanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SpecklePanel) {}

	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	~SpecklePanel();

private:
	
	//UI helper functions
	TSharedRef<SWidget> HorizontalActionsPanel();

	void Init();
	void FetchContent() const;
	
	template<typename T>
	UFUNCTION(BlueprintCallable)
	void FindAllActors(UWorld* World, TArray<T*>& Out)
	{
		for (TActorIterator<T> It(World); It; ++It)
		{
			Out.Add(*It);
		}
	}

	UPROPERTY(EditAnywhere)
	ASpeckleUnrealManager* CurrentSpeckleManager;

	UPROPERTY()
	TArray<ASpeckleUnrealManager*> SpeckleManagers;

	UPROPERTY()
	USpeckleRESTHandlerComponent* SpeckleRestHandlerComp;

	void SpeckleBranchesReceived(const TArray<FSpeckleBranch>& BranchesList);
	void SpeckleCommitsReceived(const TArray<FSpeckleCommit>& CommitsList);

	//Dropdowns and content structures
	TSharedPtr<STextComboBox> ManagersCBox;
	TSharedPtr<STextComboBox> CommitsCBox;
	TSharedPtr<STextComboBox> BranchesCBox;

	TArray<TSharedPtr<FString>> SpeckleManagerNames;
	TArray<TSharedPtr<FString>> CommitsCBoxContent;
	TArray<TSharedPtr<FString>> BranchesCBoxContent;

	TSharedPtr<FString> SelectedBranch;

	//UI handlers
	FReply ReceiveButtonClicked();

	void OnSpeckleManagersDropdownChanged(TSharedPtr<FString> SelectedName, ESelectInfo::Type InSelectionInfo);
	void OnBranchesDropdownChanged(TSharedPtr<FString> SelectedName, ESelectInfo::Type InSelectionInfo);
};
