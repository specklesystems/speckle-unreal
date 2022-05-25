// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Objects/Base.h"

#include "Mesh.generated.h"

class URenderMaterial;

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
	
	virtual bool Parse(const TSharedPtr<FJsonObject> Obj, const TScriptInterface<ITransport> ReadTransport) override;

	virtual void ToJson(TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> Writer) override;

protected:
	virtual void AlignVerticesWithTexCoordsByIndex();
};
