// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeckleUnrealProceduralMesh.h"

#include "StaticMeshDescription.h"
#include "SpeckleUnrealManager.h"
#include "Objects/RenderMaterial.h"


// Sets default values
ASpeckleUnrealProceduralMesh::ASpeckleUnrealProceduralMesh() : ASpeckleUnrealActor()
{
    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(FName("SpeckleMeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
}

void ASpeckleUnrealProceduralMesh::SetMesh_Implementation(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager)
{
    MeshComponent->ClearAllMeshSections();

    UE_LOG(LogTemp, Warning, TEXT("TESTESTESTESTST!!!"));
    TArray<int32> Faces;

    int32 j = 0;
    while (j < SpeckleMesh->Faces.Num())
    {
        int32 n = SpeckleMesh->Faces[j];
        if(n < 3) n += 3;
        
        if (n == 3) //Triangles
        {
            Faces.Add(SpeckleMesh->Faces[j + 3]);
            Faces.Add(SpeckleMesh->Faces[j + 2]);
            Faces.Add(SpeckleMesh->Faces[j + 1]);
        }
        else if(n == 4) // Quads
        {
            Faces.Add(SpeckleMesh->Faces[j + 4]);
            Faces.Add(SpeckleMesh->Faces[j + 3]);
            Faces.Add(SpeckleMesh->Faces[j + 1]);

            Faces.Add(SpeckleMesh->Faces[j + 3]);
            Faces.Add(SpeckleMesh->Faces[j + 2]);
            Faces.Add(SpeckleMesh->Faces[j + 1]);
        }
        else
        {
            // n-gons shall be ignored
        }
        
        j += n;
    }
    
    const TArray<FVector> Normals;
    const TArray<FProcMeshTangent> Tangents;
    
    MeshComponent->CreateMeshSection(
        0,
        SpeckleMesh->Vertices,
        Faces,
        Normals,
        SpeckleMesh->TextureCoordinates,
        SpeckleMesh->VertexColors,
        Tangents,
        true);
    
    MeshComponent->SetMaterial(0, CreateMaterial(Manager, SpeckleMesh->RenderMaterial));

    this->SetActorTransform(FTransform(SpeckleMesh->Transform));
}

UMaterialInterface* ASpeckleUnrealProceduralMesh::CreateMaterial(ASpeckleUnrealManager* Manager, const URenderMaterial* SpeckleMaterial)
{
    UMaterialInterface* ExplicitMaterial;
    if(SpeckleMaterial->Opacity >= 1)
        ExplicitMaterial = Manager->BaseMeshOpaqueMaterial;
    else
        ExplicitMaterial = Manager->BaseMeshTransparentMaterial;
		
    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(ExplicitMaterial, Manager, FName(SpeckleMaterial->Name));
	
    DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
    DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
    DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
    DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
    DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
	
    Manager->ConvertedMaterials.Add(SpeckleMaterial->Id, DynMaterial);
	
    return DynMaterial;
	
}
