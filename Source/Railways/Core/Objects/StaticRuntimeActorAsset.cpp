// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticRuntimeActorAsset.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#include "../RailwaysGameInstance.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "../Helpers/stb_image.h"

void AStaticRuntimeActorAsset::BeginPlay()
{
    Super::BeginPlay();
    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        MeshData = GameInstance->AssimpInterface->ImportFBX(false);
        InitAsset();
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

void AStaticRuntimeActorAsset::BuildMeshNode(const AssimpNode& Node)
{
    const int32 NumMeshes = Node.Meshes.Num();
    static int MeshIndex = 0;
    for (const AssimpMesh* Mesh : Node.Meshes)
    {
        //const AssimpMesh* Mesh = Node.Meshes[i];
        TArray<FVector> Positions;
        TArray<FVector> Normals;
        TArray<FVector2D> UVs;
        TArray<FColor> Colors;
        TArray<FProcMeshTangent> Tangents;

        Positions.AddUninitialized(Mesh->Vertices.Num());
        Normals.AddUninitialized(Mesh->Vertices.Num());
        UVs.AddUninitialized(Mesh->Vertices.Num());
        Colors.AddUninitialized(Mesh->Vertices.Num());
        Tangents.AddUninitialized(Mesh->Vertices.Num());

        for (int vert = 0; vert < Mesh->Vertices.Num(); vert++)
        {
            const AssimpVert& VertData = Mesh->Vertices[vert];
            Positions[vert] = VertData.Location * 100.0f;
            //Positions[vert] = VertData.Location * 10.0f;
            //Positions[vert] = VertData.Location;
            Normals[vert] = VertData.Normal;
            UVs[vert] = VertData.TexCoords;
            Colors[vert] = FColor(255, 255, 255);
            Tangents[vert] = FProcMeshTangent(VertData.Tangent, false);
        }

        TArray<int32> Triangles;
        Triangles.AddUninitialized(Mesh->Elements.Num());
        for (int tri = 0; tri < Mesh->Elements.Num(); tri++)
        {
            Triangles[tri] = Mesh->Elements[tri];
        }

        //create material
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PBRMaterial, this);

        //create textures
        for (const auto& Texture : Mesh->Material.Textures)
        {
            int width, height, nrComponents;
            unsigned char* data = stbi_load(TCHAR_TO_UTF8(*Texture.Path), &width, &height, &nrComponents, 4);
            if (data)
            {
                FName TextureName = MakeUniqueObjectName(this, UTexture2D::StaticClass());
                UTexture2D* NewTexture = NewObject<UTexture2D>(this, TextureName, RF_Transient);

                EPixelFormat format = PF_R8G8B8A8;
                NewTexture->PlatformData = new FTexturePlatformData();
                NewTexture->PlatformData->SizeX = width;
                NewTexture->PlatformData->SizeY = height;
                NewTexture->PlatformData->PixelFormat = format;
                if (Texture.Type == TextureType::Normal || Texture.Type == TextureType::Parameter)
                {
                    NewTexture->SRGB = false;
                }
                //mipmaps
                int32 NumBlocksX = width;
                int32 NumBlocksY = height;
                FTexture2DMipMap* Mip = new(NewTexture->PlatformData->Mips) FTexture2DMipMap();
                Mip->SizeX = width;
                Mip->SizeY = height;
                Mip->BulkData.Lock(LOCK_READ_WRITE);
                void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * 4); //GPixelFormats[format].BlockBytes
                FMemory::Memcpy(TextureData, data, width * height * 4);
                Mip->BulkData.Unlock();

                NewTexture->UpdateResource();

                stbi_image_free(data);

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

        MeshComponent->SetMaterial(MeshIndex, DynMaterial);

        MeshComponent->CreateMeshSection(MeshIndex, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
        //DynamicMesh->CreateMeshSection(MeshIndex, Positions, Triangles, Normals, UVs, Tangents);
        MeshIndex++;
    }

    for (const AssimpNode& Child : Node.Children)
        BuildMeshNode(Child);
}

void AStaticRuntimeActorAsset::InitAsset()
{
    BuildMeshNode(*MeshData->RootNode);
}