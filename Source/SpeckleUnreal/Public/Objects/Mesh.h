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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Vertices;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> Faces;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> TextureCoordinates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FColor> VertexColors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMatrix Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URenderMaterial* RenderMaterial;
	
	virtual void Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager) override;

protected:
	virtual void AlignVerticesWithTexCoordsByIndex();
};
