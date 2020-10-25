// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticRuntimeActorAsset.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#include "../RailwaysGameInstance.h"
#include "DrawDebugHelpers.h"

void AStaticRuntimeActorAsset::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BEGIN STATIC ASSET"));
    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        //MeshData = GameInstance->AssimpInterface->ImportFBX(false);
        MeshContent.MeshData = GameInstance->AssimpInterface->ImportFBX(false);
        InitAsset();
        //MeshContent.SaveMesh(TEXT("test"));
    }
}

// Called every frame
void AStaticRuntimeActorAsset::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

AStaticRuntimeActorAsset::AStaticRuntimeActorAsset()
{
    MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);
}

AStaticRuntimeActorAsset::~AStaticRuntimeActorAsset()
{
}

void AStaticRuntimeActorAsset::BuildMeshNode(const RailwaysNode& Node)
{
    const int32 NumMeshes = Node.Meshes.Num();
    static int MeshIndex = 0;
    for (const RailwaysMesh& Mesh : Node.Meshes)
    {
        //const AssimpMesh* Mesh = Node.Meshes[i];
        TArray<FVector> Positions;
        TArray<FVector> Normals;
        TArray<FVector2D> UVs;
        TArray<FColor> Colors;
        TArray<FProcMeshTangent> Tangents;

        Positions.AddUninitialized(Mesh.Vertices.Num());
        Normals.AddUninitialized(Mesh.Vertices.Num());
        UVs.AddUninitialized(Mesh.Vertices.Num());
        Colors.AddUninitialized(Mesh.Vertices.Num());
        Tangents.AddUninitialized(Mesh.Vertices.Num());

        for (int vert = 0; vert < Mesh.Vertices.Num(); vert++)
        {
            const RailwaysVert& VertData = Mesh.Vertices[vert];
            //Positions[vert] = VertData.Location * 100.0f;
            //Positions[vert] = VertData.Location * 10.0f;
            Positions[vert] = VertData.Location;
            //Normals[vert] = VertData.Normal.GetSafeNormal();
            Normals[vert] = VertData.Normal;
            UVs[vert] = VertData.TexCoords;
            Colors[vert] = FColor(255, 255, 255);
            Tangents[vert] = FProcMeshTangent(VertData.Tangent, false);
        }

        TArray<int32> Triangles;
        Triangles.AddUninitialized(Mesh.Elements.Num());
        for (int tri = 0; tri < Mesh.Elements.Num(); tri++)
        {
            Triangles[tri] = Mesh.Elements[tri];
        }

        MeshComponent->SetMaterial(MeshIndex, MaterialInstances[Mesh.MaterialIndex]);

        MeshComponent->CreateMeshSection(MeshIndex, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
        //DynamicMesh->CreateMeshSection(MeshIndex, Positions, Triangles, Normals, UVs, Tangents);
        MeshIndex++;
    }

    for (const RailwaysNode& Child : Node.Children)
        BuildMeshNode(Child);
}

void AStaticRuntimeActorAsset::InitAsset()
{
    //material setup
    for (const auto& Material : MeshContent.MeshData->Materials)
    {
        //create material
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PBRMaterial, this);

        //create textures
        for (const auto& Texture : Material.Textures)
        {
            bool SRGB = !(Texture.Type == TextureType::Normal || Texture.Type == TextureType::Parameter);

            UTexture2D* NewTexture = LoadTextureFile(Texture.Path, SRGB);

            if (NewTexture)
            {
                FName TextureTarget;
                switch (Texture.Type)
                {
                default:
                case TextureType::Diffuse:
                    TextureTarget = FName(TEXT("BaseColor"));
                    break;
                case TextureType::Normal:
                    TextureTarget = FName(TEXT("Normal"));
                    break;
                case TextureType::Parameter:
                    TextureTarget = FName(TEXT("Parameter"));
                    break;
                }

                DynMaterial->SetTextureParameterValue(TextureTarget, NewTexture);
            }
        }

        MaterialInstances.Add(DynMaterial);
    }

    BuildMeshNode(*MeshContent.MeshData->RootNode);
}