// Fill out your copyright notice in the Description page of Project Settings.

#include "Conversion/Converters/ProceduralMeshConverter.h"

#include "ProceduralMeshComponent.h"
#include "StaticMeshDescription.h"
#include "SpeckleUnrealManager.h"
#include "Conversion/Converters/RenderMaterialConverter.h"
#include "Objects/Mesh.h"
#include "Objects/RenderMaterial.h"

UProceduralMeshConverter::UProceduralMeshConverter()
{
    SpeckleTypes.Add(UMesh::StaticClass());
    
    MeshActorType = AActor::StaticClass();
    ActorMobility = EComponentMobility::Static;
}

AActor* UProceduralMeshConverter::ConvertToNative_Implementation(const UBase* SpeckleBase, ASpeckleUnrealManager* Manager)
{
    const UMesh* P = Cast<UMesh>(SpeckleBase);
	
    if(P == nullptr) return nullptr;
	
    return MeshToNative(P, Manager);
}

AActor* UProceduralMeshConverter::MeshToNative(const UMesh* SpeckleMesh, ASpeckleUnrealManager* Manager)
{
    AActor* MeshActor = CreateEmptyActor(Manager, FTransform(SpeckleMesh->Transform));
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

    UMaterialInterface* Material = MaterialConverter->GetMaterial(SpeckleMesh->RenderMaterial, Manager, true, false);
    MeshComponent->SetMaterial(0, Material);
    
    return MeshActor;
}

AActor* UProceduralMeshConverter::CreateEmptyActor(const ASpeckleUnrealManager* Manager, const FTransform& Transform, const FActorSpawnParameters& SpawnParameters)
{
    AActor* Actor = Manager->GetWorld()->SpawnActor<AActor>(MeshActorType, Transform, SpawnParameters);
    USceneComponent* Scene = NewObject<USceneComponent>(Actor, "Root");
    Actor->SetRootComponent(Scene);
    Scene->RegisterComponent();
    Scene->SetMobility(ActorMobility);
    return Actor;
}

void UProceduralMeshConverter::CleanUp_Implementation()
{
    MaterialConverter->CleanUp();
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
