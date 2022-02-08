// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "CoreMinimal.h"
// #include "UObject/Object.h"
//
// #include "AbstractTransport.generated.h"
//
// /**
//  * 
//  */
// UCLASS(Abstract)
// class SPECKLEUNREAL_API UAbstractTransport : public UObject
// {
// 	GENERATED_BODY()
// 	
// protected:
// 	explicit UAbstractTransport(const FObjectInitializer& Init);
// 	explicit UAbstractTransport(TMap<FString, TSharedPtr<FJsonObject>>& Objects);
//
// public:
// 	virtual void SaveObject(FString& Id, TSharedPtr<FJsonObject> SerializedObject);
//
// 	virtual void SaveObjectFromTransport(FString& Id, UAbstractTransport* SourceTransport);
//
// 	virtual TSharedPtr<FJsonObject, ESPMode::Fast> GetObject(const FString& Id);
//
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	virtual bool HasObject(const FString& Id) const;
// 	
// 	
// 	virtual TSharedPtr<FJsonObject> CopyObjectAndChildren(FString& Id, UAbstractTransport* SourceTransport);
//
//
// public: //Helper methods
// 	
//
//
//
// };
