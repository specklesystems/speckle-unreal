// Fill out your copyright notice in the Description page of Project Settings.


#include "FSpeckleEditorCommands.h"
#include "Framework/Commands/Commands.h"

void FSpeckleEditorCommands::RegisterCommands()
{
	//we meed to define a localized namespace even if its not used
	#define LOCTEXT_NAMESPACE ""
	
		UI_COMMAND(
			SpeckleToolBarBtn,
			"SpeckleBtn",
			"Speckle Editor Button",
			EUserInterfaceActionType::Button, 
			FInputGesture());
	
	#undef LOCTEXT_NAMESPACE
}
