// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Mesh.h"

#include "SpeckleUnrealManager.h"
#include "Objects/RenderMaterial.h"

bool UMesh::Parse(const TSharedPtr<FJsonObject> Obj, const ASpeckleUnrealManager* Manager)
{
	if(!Super::Parse(Obj, Manager)) return false;
	const float ScaleFactor = Manager->ParseScaleFactor(Units);

	//Parse optional Transform
	{
		Transform = FMatrix::Identity;

		const TArray<TSharedPtr<FJsonValue>>* TransformData = nullptr;
		if(Obj->HasField("properties") && Obj->GetObjectField("properties")->TryGetArrayField("transform", TransformData))
		{
			for(int32 Row = 0; Row < 4; Row++)
				for(int32 Col = 0; Col < 4; Col++)
				{
					Transform.M[Row][Col] = TransformData->operator[](Row * 4 + Col)->AsNumber();
				}
			Transform = Transform.GetTransposed();
			Transform.ScaleTranslation(FVector(ScaleFactor));
		}
	}

	//Parse Vertices
	{
		TArray<TSharedPtr<FJsonValue>> ObjectVertices = Manager->CombineChunks(Obj->GetArrayField("vertices"));
		const int32 NumberOfVertices = ObjectVertices.Num() / 3;

		Vertices.Reserve(NumberOfVertices);

		for (size_t i = 0, j = 0; i < NumberOfVertices; i++, j += 3)
		{
			Vertices.Add(Transform.InverseTransformPosition(FVector
			(
				ObjectVertices[j].Get()->AsNumber(),
				ObjectVertices[j + 1].Get()->AsNumber(),
				ObjectVertices[j + 2].Get()->AsNumber()
			) * ScaleFactor ));
		}
		DynamicProperties.Remove("vertices");
	}

	//Parse Faces
	{
		const TArray<TSharedPtr<FJsonValue>> FaceVertices = Manager->CombineChunks(Obj->GetArrayField("faces"));
		Faces.Reserve(FaceVertices.Num());
		for(const auto VertIndex : FaceVertices)
		{
			Faces.Add(VertIndex->AsNumber());
		}
		DynamicProperties.Remove("faces");
	}

	//Parse TextureCoords
	{
		const TArray<TSharedPtr<FJsonValue>>* TextCoordArray;
		if(Obj->TryGetArrayField("textureCoordinates", TextCoordArray))
		{
			TArray<TSharedPtr<FJsonValue>> TexCoords = Manager->CombineChunks(*TextCoordArray);
	
			TextureCoordinates.Reserve(TexCoords.Num() / 2);
	
			for (int32 i = 0; i + 1 < TexCoords.Num(); i += 2)
			{
				TextureCoordinates.Add(FVector2D
				(
					TexCoords[i].Get()->AsNumber(),
					TexCoords[i + 1].Get()->AsNumber()
				)); 
			}
			DynamicProperties.Remove("textureCoordinates");
		}
	}

	//Parse VertexColors
	{
		const TArray<TSharedPtr<FJsonValue>>* ColorArray;
		if(Obj->TryGetArrayField("colors", ColorArray))
		{
			TArray<TSharedPtr<FJsonValue>> Colors = Manager->CombineChunks(*ColorArray);
	
			VertexColors.Reserve(Colors.Num());
	
			for (int32 i = 0; i + 1 < Colors.Num(); i ++)
			{
				VertexColors.Add(FColor(Colors[i].Get()->AsNumber()));
			}
			DynamicProperties.Remove("colors");
		}
	}

	//Parse Optional RenderMaterial
	if (Obj->HasField("renderMaterial"))
	{
		RenderMaterial = NewObject<URenderMaterial>();
		RenderMaterial->Parse(Obj->GetObjectField("renderMaterial"), Manager);
		DynamicProperties.Remove("renderMaterial");
	}
	
	AlignVerticesWithTexCoordsByIndex();

	return Vertices.Num() > 0 && Faces.Num() > 0;
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

	TArray<int> FacesUnique;
	FacesUnique.Reserve(Faces.Num());
	TArray<FVector> VerticesUnique;
	VerticesUnique.Reserve(TextureCoordinates.Num());
	const bool HasColor = VertexColors.Num() > 0;
	TArray<FColor> ColorsUnique;
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

			if(HasColor) ColorsUnique.Add(VertexColors[NewVertIndex]);
			FacesUnique.Add(NewVertIndex);
		}
		NIndex += n + 1;
	}
	
	Vertices = VerticesUnique;
	VertexColors = ColorsUnique;
	Faces = FacesUnique;
	
}