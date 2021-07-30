// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "LevelEditor.h"
#include "SlateBasics.h"
#include "Interfaces/IMainFrameModule.h"
#include "FSpeckleEditorCommands.h"

class FSpeckleUnrealEditorModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	TSharedPtr<FExtender> ToolbarExtender;
    TSharedPtr<const FExtensionBase> Extension;
    TSharedPtr<SWindow> SpeckleEditorWindow;
    FOnWindowClosed OnSpeckleWindowClosed;
    
    void SpeckleButtonListener();

    UFUNCTION()
    void OnEditorWindowClosed(const TSharedRef<SWindow>&);
    
    void AddToolbarExtension(FToolBarBuilder &builder)
    {
    	FSlateIcon IconBrush = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions", "LevelEditor.ViewOptions.Small");
    	
    	builder.AddToolBarButton
    	(
    		FSpeckleEditorCommands::Get().SpeckleToolBarBtn,
    		NAME_None,
    	    FText::FromString("Speckle Editor"), 
    	    FText::FromString("Click to open Speckle Window"),
    	    IconBrush,
    	    NAME_None
        );
    };
};
