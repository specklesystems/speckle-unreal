// Fill out your copyright notice in the Description page of Project Settings.

#include "NativeActors/SpeckleUnrealProceduralMesh.h"

#include "StaticMeshDescription.h"
#include "SpeckleUnrealManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Objects/Mesh.h"
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
    
    TArray<int32> Faces;

    int32 i = 0;
    while (i < SpeckleMesh->Faces.Num())
    {
        int32 n = SpeckleMesh->Faces[i];
        if(n < 3) n += 3; // 0 -> 3, 1 -> 4
        
        if (n == 3) //Triangles
        {
            Faces.Add(SpeckleMesh->Faces[i + 3]);
            Faces.Add(SpeckleMesh->Faces[i + 2]);
            Faces.Add(SpeckleMesh->Faces[i + 1]);
        }
        else if(n == 4) // Quads
        {
            Faces.Add(SpeckleMesh->Faces[i + 4]);
            Faces.Add(SpeckleMesh->Faces[i + 3]);
            Faces.Add(SpeckleMesh->Faces[i + 1]);

            Faces.Add(SpeckleMesh->Faces[i + 3]);
            Faces.Add(SpeckleMesh->Faces[i + 2]);
            Faces.Add(SpeckleMesh->Faces[i + 1]);
        }
        else
        {
            // n-gons shall be ignored
        }
        
        i += n + 1;
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
    
    MeshComponent->SetMaterial(0, GetMaterial(SpeckleMesh->RenderMaterial, Manager));
}

UMaterialInterface* ASpeckleUnrealProceduralMesh::GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager)
{
    UMaterialInterface* ExistingMaterial;
    if(Manager->TryGetMaterial(SpeckleMaterial, true, ExistingMaterial))
        return ExistingMaterial; //Return existing material
		
    UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
        ? Manager->BaseMeshOpaqueMaterial
        : Manager->BaseMeshTransparentMaterial;

    const FString PackagePath = FPaths::Combine(TEXT("/Game/Speckle"), Manager->StreamID, TEXT("Materials"), SpeckleMaterial->Id);
    UPackage* Package = CreatePackage(*PackagePath);
	
    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialBase, Package, FName(SpeckleMaterial->Name));
    
    DynMaterial->SetFlags(RF_Public);
	
    DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
    DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
    DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
    DynMaterial->SetVectorParameterValue("BaseColor", SpeckleMaterial->Diffuse);
    DynMaterial->SetVectorParameterValue("EmissiveColor", SpeckleMaterial->Emissive);
	
    Manager->ConvertedMaterials.Add(SpeckleMaterial->Id, DynMaterial);

#if WITH_EDITOR
    FAssetRegistryModule::AssetCreated(DynMaterial);
#endif
    
    return DynMaterial;
	
}