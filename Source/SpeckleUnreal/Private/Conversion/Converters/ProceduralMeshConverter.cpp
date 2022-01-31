// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/ProceduralMeshConverter.h"

#include "ProceduralMeshComponent.h"
#include "StaticMeshDescription.h"
#include "SpeckleUnrealManager.h"
#include "Objects/Mesh.h"
#include "Objects/RenderMaterial.h"

UProceduralMeshConverter::UProceduralMeshConverter()
{
    SpeckleTypes.Add(UMesh::StaticClass());
    MeshActorType = AActor::StaticClass();
    bCreateCollisions = true;
}

AActor* UProceduralMeshConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager)
{
    const UMesh* P = Cast<UMesh>(SpeckleBase);
	
    if(P == nullptr) return nullptr;

    //No existing mesh was found, try and convert SpeckleMesh
    UMesh* ScaledMesh = DuplicateObject(P, P->GetOuter(), P->GetFName());
    ScaledMesh->ApplyUnits(Manager->GetWorld());
    ScaledMesh->AlignVerticesWithTexCoordsByIndex();
    
    return MeshToNative(ScaledMesh, Manager);
}

AActor* UProceduralMeshConverter::MeshToNative(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager)
{
    AActor* MeshActor = CreateActor(Manager, FTransform(SpeckleMesh->GetTransform()));
    UProceduralMeshComponent* MeshComponent = NewObject<UProceduralMeshComponent>(MeshActor, FName("SpeckleMeshComponent"));
    MeshComponent->SetupAttachment(MeshActor->GetRootComponent());
    MeshComponent->RegisterComponent();
    
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

    TArray<FColor> VertexColors;
    VertexColors.Reserve(SpeckleMesh->Colors.Num());
    for(const int32& c : SpeckleMesh->Colors) VertexColors.Add(FColor(c));
    
    const TArray<FVector> Normals;
    const TArray<FProcMeshTangent> Tangents;
    
    MeshComponent->CreateMeshSection(
        0,
        SpeckleMesh->GetVerts(),
        Faces,
        Normals,
        SpeckleMesh->GetTextureCoordinates(),
        VertexColors,
        Tangents,
        bCreateCollisions);
    
    MeshComponent->SetMaterial(0, GetMaterial(SpeckleMesh->RenderMaterial, Manager));
    
    return MeshActor;
}

AActor* UProceduralMeshConverter::CreateActor(const ASpeckleUnrealManager* Manager, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
    AActor* Actor = Manager->GetWorld()->SpawnActor<AActor>(MeshActorType, Transform, SpawnParameters);
    USceneComponent* Scene = NewObject<USceneComponent>(Actor, "Root");
    Actor->SetRootComponent(Scene);
    Scene->RegisterComponent();
    return Actor;
}


UMaterialInterface* UProceduralMeshConverter::GetMaterial(const URenderMaterial* SpeckleMaterial, ASpeckleUnrealManager* Manager)
{
    UMaterialInterface* ExistingMaterial;
    if(Manager->TryGetMaterial(SpeckleMaterial, true, ExistingMaterial))
        return ExistingMaterial; //Return existing material
		
    UMaterialInterface* MaterialBase = SpeckleMaterial->Opacity >= 1
        ? Manager->BaseMeshOpaqueMaterial
        : Manager->BaseMeshTransparentMaterial;
	
    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MaterialBase, Manager, FName(SpeckleMaterial->Name));
    
    DynMaterial->SetFlags(RF_Public);
	
    DynMaterial->SetScalarParameterValue("Opacity", SpeckleMaterial->Opacity);
    DynMaterial->SetScalarParameterValue("Metallic", SpeckleMaterial->Metalness);
    DynMaterial->SetScalarParameterValue("Roughness", SpeckleMaterial->Roughness);
    DynMaterial->SetVectorParameterValue("BaseColor", FColor(SpeckleMaterial->Diffuse));
    DynMaterial->SetVectorParameterValue("EmissiveColor", FColor(SpeckleMaterial->Emissive));
	
    Manager->ConvertedMaterials.Add(SpeckleMaterial->Id, DynMaterial);
    
    return DynMaterial;
}


UBase* UProceduralMeshConverter::ConvertToSpeckle_Implementation(const UObject* Object, ASpeckleUnrealManager* Manager)
{
    const UProceduralMeshComponent* M = Cast<UProceduralMeshComponent>(Object);

    if(M == nullptr)
    {
        const AActor* A = Cast<AActor>(Object);
        if(A != nullptr)
        {
            M = A->FindComponentByClass<UProceduralMeshComponent>();
        }
    }
    if(M == nullptr) return nullptr;
	
    return MeshToSpeckle(M, Manager);
}


UMesh* UProceduralMeshConverter::MeshToSpeckle(const UProceduralMeshComponent* Object, ASpeckleUnrealManager* Manager)
{
    return nullptr; //TODO implement ToSpeckle function
}
