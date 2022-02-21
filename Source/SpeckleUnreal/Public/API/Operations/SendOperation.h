// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Kismet/BlueprintAsyncActionBase.h"
//
// #include "SendOperation.generated.h"
//
//
// class ITransport;
// class UBase;
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSendOperationHandler, const FString&, Id, const FString&, ErrorMessage);
//
// /**
//  * 
//  */
// UCLASS()
// class SPECKLEUNREAL_API USendOperation : public UBlueprintAsyncActionBase
// {
// 	GENERATED_BODY()
// 	
// public:
// 	
// 	UPROPERTY(BlueprintAssignable)
// 	FSendOperationHandler OnSendSuccessfully;
// 	
// 	
// 	UPROPERTY(BlueprintAssignable)
// 	FSendOperationHandler OnErrorAction;
//
//
// 	UFUNCTION(BlueprintCallable, Category = "Speckle|Operations", meta = (WorldContext = "WorldContextObject"))
// 	static USendOperation* SendOperation(UObject* WorldContextObject, UBase* Base, TArray<TScriptInterface<ITransport>> Transports);
// 	
// 	virtual void Activate() override;
// 	
// protected:
//
// 	
// 	TWeakObjectPtr<UBase> Base;
// 	
// 	TScriptArray<TScriptInterface<ITransport>> Transports;
//
// 	
// };