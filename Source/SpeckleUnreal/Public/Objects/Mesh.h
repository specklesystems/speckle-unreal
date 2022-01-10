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

	UMesh() : UBase(TEXT("Objects.Other.Mesh")) {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FVector> Vertices;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<int32> Faces;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FVector2D> TextureCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	TArray<FColor> VertexColors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	FMatrix Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Speckle|Objects")
	URenderMaterial* RenderMaterial;
	
	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;

protected:
	virtual void AlignVerticesWithTexCoordsByIndex();
};
