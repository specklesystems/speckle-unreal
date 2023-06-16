
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"

#include "ObjectModelRegistry.generated.h"

class UBase;

/**
 * Handles the mapping of Speckle type to Object Model 
 */
UCLASS()
class SPECKLEUNREAL_API UObjectModelRegistry : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
private:

	static TMap<FString, TSubclassOf<UBase>> TypeRegistry;

	static void GenerateTypeRegistry();

public:
	
	/**
	 * @brief Searches for a closest registered speckle type
	 * by recursively stripping away a the most specific name specifier from the given SpeckleType
	 * e.g. With an input of `"Objects.BuiltElements.Wall:Objects.BuiltElements.RevitWall"` will first search for
	 * Will first look for a registered type of `"Objects.BuiltElements.RevitWall"`, then if not found `"Objects.BuiltElements.Wall"`, then simply will use UBase
	 * @param SpeckleType The full speckle type
	 * @return The closest registered type
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> GetAtomicType(const FString& SpeckleType);
	

	// E.g. "Objects.BuiltElements.Wall:Objects.BuiltElements.RevitWall" -> "Objects.BuiltElements.RevitWall"
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Speckle|Objects")
	static FString GetTypeName(const FString& SpeckleType);

	// E.g. "Objects.BuiltElements.Wall:Objects.BuiltElements.RevitWall" -> "RevitWall"
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Speckle|Objects")
	static FString GetSimplifiedTypeName(const FString& SpeckleType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Speckle|Objects")
	static bool SplitSpeckleType(const FString& SpeckleType, FString& OutRemainder, FString& OutTypeName, const FString& Split = ":");
	
	/**
	* @brief Attempts to find a `TSubclassOf<UBase>` with a `UBase::SpeckleType` matching the given SpeckleType param
	 * @param TypeName 
	 * @return the matching type or `nullptr` if none found.
	 */
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static TSubclassOf<UBase> GetRegisteredType(const FString& TypeName);

	/// Attempts to find a TSubclassOf<UBase> with a UBase::SpeckleType matching the given SpeckleType param
	UFUNCTION(BlueprintCallable, Category="Speckle|Objects")
	static bool TryGetRegisteredType(const FString& TypeName, TSubclassOf<UBase>& OutType);
};