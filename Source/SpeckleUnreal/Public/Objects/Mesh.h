// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base.h"

#include "Mesh.generated.h"

class URenderMaterial;
class ASpeckleUnrealManager;
/**
 * 
 */
UCLASS()
class SPECKLEUNREAL_API UMesh : public UBase
{
	GENERATED_BODY()
	
public:

	UMesh() : UBase(TEXT("Objects.Geometry.Mesh")) {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<float> Vertices;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<int32> Faces;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<float> TextureCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<int32> Colors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<float> Transform;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	URenderMaterial* RenderMaterial;

public:
	
	UFUNCTION(BlueprintPure)
	FVector GetVert(int32 Index) const;
	UFUNCTION(BlueprintPure)
	TArray<FVector> GetVerts() const;
	UFUNCTION(BlueprintPure)
	int32 GetVertexCount() const { return Vertices.Num() / 3; }
	
	UFUNCTION(BlueprintPure)
	FVector2D GetTextureCoordinate(int32 Index) const;
	UFUNCTION(BlueprintPure)
    TArray<FVector2D> GetTextureCoordinates() const;
	UFUNCTION(BlueprintPure)
	int32 GetTexCoordCount() const { return TextureCoordinates.Num() / 2; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE FColor GetVertexColor(int32 Index) const;
	UFUNCTION(BlueprintPure)
	TArray<FColor> GetVertexColors() const;
	
	UFUNCTION(BlueprintPure)
	FMatrix GetTransform() const;
	UFUNCTION(BlueprintCallable)
	void SetTransform(const FMatrix& T);
	
	UFUNCTION()
	virtual void AlignVerticesWithTexCoordsByIndex();

	UFUNCTION()
	virtual void ApplyScaleFactor(const float ScaleFactor);

	UFUNCTION()
	virtual void ApplyUnits(const UWorld* World);
	
};
