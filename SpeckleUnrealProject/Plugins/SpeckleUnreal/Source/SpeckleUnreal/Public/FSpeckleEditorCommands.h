// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/**
 * 
 */
class SPECKLEUNREAL_API FSpeckleEditorCommands : public TCommands<FSpeckleEditorCommands>
{
public:
	FSpeckleEditorCommands() :TCommands<FSpeckleEditorCommands>(FName(TEXT("Speckle Commands")), 
   FText::FromString("Speckle Editor Commands"), NAME_None, 
   FEditorStyle::GetStyleSetName())
	{	
	};
	
	virtual void RegisterCommands() override;
	
	TSharedPtr<FUICommandInfo> SpeckleToolBarBtn;
};
