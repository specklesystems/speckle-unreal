// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Mesh.h"

#include "SpeckleUnrealManager.h"
#include "Conversion/ConversionUtils.h"

FMatrix UMesh::GetTransform() const
{
	if(Transform.Num() != 16) return FMatrix::Identity;
	
	FMatrix TransformMatrix;
		
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			TransformMatrix.M[Row][Col] = Transform[Row * 4 + Col];
		}
	
	TransformMatrix = TransformMatrix.GetTransposed();

	return TransformMatrix;	
}

void UMesh::SetTransform(const FMatrix& T)
{
	const FMatrix TransformMatrix = T.GetTransposed();
	
	for(int32 Row = 0; Row < 4; Row++)
		for(int32 Col = 0; Col < 4; Col++)
		{
			Transform[Row * 4 + Col] = TransformMatrix.M[Row][Col];
		}
}



FVector UMesh::GetVert(int32 Index) const
{
	Index *= 3;
	return FVector(
	  Vertices[Index],
	  Vertices[Index + 1],
	  Vertices[Index + 2]
	  );
}


TArray<FVector> UMesh::GetVerts() const
{
	check(Vertices.Num() % 3 == 0);
	
	//TODO - Maybe could just use a blit copy assuming 3 floats -> FVector
	TArray<FVector> VertexVectors;

	const int32 NumberOfVertices = Vertices.Num() / 3;

	VertexVectors.Reserve(NumberOfVertices);

	for (size_t i = 0, j = 0; i < NumberOfVertices; i++, j += 3)
	{
		VertexVectors.Add(FVector
		(
			Vertices[j],
			Vertices[j + 1],
			Vertices[j + 2]
		));
	}
	return VertexVectors;
}


FVector2D UMesh::GetTextureCoordinate(int32 Index) const
{
	Index *= 2;
	return FVector2D(TextureCoordinates[Index], TextureCoordinates[Index + 1]);
}

TArray<FVector2D> UMesh::GetTextureCoordinates() const
{
	//TODO - Maybe could just use a blit copy assuming 2 floats -> FVector2D
	TArray<FVector2D> TexCoords;
	
	TexCoords.Reserve(TextureCoordinates.Num() / 2);

	for (int32 i = 0; i + 1 < TexCoords.Num(); i += 2)
	{
		TexCoords.Add(FVector2D
		(
			TextureCoordinates[i],
			TextureCoordinates[i + 1]
		)); 
	}
	return TexCoords;
}

FColor UMesh::GetVertexColor(int32 Index) const
{
	return FColor(Colors[Index]);
}

TArray<FColor> UMesh::GetVertexColors() const
{
	TArray<FColor> VertexColors;
	
	VertexColors.Reserve(Colors.Num());

	for (int32 i = 0; i + 1 < Colors.Num(); i ++)
	{
		VertexColors.Add(FColor(Colors[i]));
	}
	
	return VertexColors;
}


/**
 * If not already so, this method will align vertices
 * such that a vertex and its corresponding texture coordinates have the same index.
 * See "https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/Geometry/Mesh.cs"
 */
void UMesh::AlignVerticesWithTexCoordsByIndex()
{
	if(TextureCoordinates.Num() == 0) return;
	if(TextureCoordinates.Num() == Vertices.Num()) return; //Tex-coords already aligned as expected

	TArray<int32> FacesUnique;
	FacesUnique.Reserve(Faces.Num());
	TArray<float> VerticesUnique;
	VerticesUnique.Reserve(TextureCoordinates.Num() * 3);
	const bool HasColor = Colors.Num() > 0;
	TArray<int32> ColorsUnique;
	if(HasColor) ColorsUnique.Reserve(TextureCoordinates.Num());

	int32 NIndex = 0;
	while(NIndex < Faces.Num())
	{
		int32 n = Faces[NIndex];
		if (n < 3) n += 3; // 0 -> 3, 1 -> 4

		if (NIndex + n >= Faces.Num()) break; //Malformed face list

		FacesUnique.Add(n);

		for (int32 i = 1; i <= n; i++)
		{
			const int32 VertIndex = Faces[NIndex + i];
			const int32 NewVertIndex = VerticesUnique.Num();
			
			VerticesUnique.Add(Vertices[VertIndex]);

			if(HasColor) ColorsUnique.Add(Colors[NewVertIndex]);
			FacesUnique.Add(NewVertIndex);
		}
		NIndex += n + 1;
	}
	
	Vertices = VerticesUnique;
	Colors = ColorsUnique;
	Faces = FacesUnique;
	
}

void UMesh::ApplyScaleFactor(const float ScaleFactor)
{
	for (size_t i = 0; i < Vertices.Num(); i++)
	{
		Vertices[i] *= ScaleFactor;
	}

	FMatrix Transform = GetTransform();
	Transform.ScaleTranslation(FVector(ScaleFactor));
	
}

void UMesh::ApplyUnits(const UWorld* World)
{
	const UWorld* CheckedWorld = IsValid(World)? World : GetWorld();
	const float ScaleFactor = UConversionUtils::GetUnitsScaleFactor(Units, CheckedWorld);
	ApplyScaleFactor(ScaleFactor);
	
	Units = "cm";
}
