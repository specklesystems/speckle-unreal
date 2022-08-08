
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

protected:
	

	/// If not already so, this method will align vertices
	/// such that a vertex and its corresponding texture coordinates have the same index.
	/// See "https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/Geometry/Mesh.cs"
	virtual void AlignVerticesWithTexCoordsByIndex();
};
