// Copyright AEC Systems Ltd

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DynamicBase.generated.h"



class UBase;
/**
 * 
 */
UCLASS(Abstract)
class SPECKLEUNREAL_API UDynamicBase : public UObject
{
	GENERATED_BODY()

protected:
	
	TMap<FString, TSharedPtr<FVariant>> DynamicProperties;

public:
	
	template <typename T>
	bool TryGetDynamicProperty(const FString& Key, T& OutValue) const;
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicStringProperty(UPARAM(ref) const FString& Key, FString& OutValue) const;
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicIntProperty(UPARAM(ref) const FString& Key, int32& OutValue) const;
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicFloatProperty(UPARAM(ref) const FString& Key, float& OutValue) const;
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicBoolProperty(UPARAM(ref) const FString& Key, bool& OutValue) const;

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	bool TryGetDynamicBaseProperty(UPARAM(ref) const FString& Key, UBase*& OutValue) const;

	
	template <typename T>
	void SetDynamicProperty(const FString& Key, T Value);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	void SetDynamicStringProperty(UPARAM(ref) const FString& Key, FString& OutValue);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	void SetDynamicIntProperty(UPARAM(ref) const FString& Key, int32& OutValue);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	void SetDynamicFloatProperty(UPARAM(ref) const FString& Key, float& OutValue);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	void SetDynamicBoolProperty(UPARAM(ref) const FString& Key, bool& OutValue);

	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	void SetDynamicBaseProperty(UPARAM(ref) const FString& Key, UBase*& OutValue);
	
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	int32 RemoveDynamicProperty(UPARAM(ref) const FString& Key);

};
