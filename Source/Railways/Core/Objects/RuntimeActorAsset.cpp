// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Helpers/stb_image.h"
#include "../RailwaysGameInstance.h"

// Sets default values
ARuntimeActorAsset::ARuntimeActorAsset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARuntimeActorAsset::InitMesh()
{
    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        auto Data = GameInstance->AssimpInterface->ImportFBX();

        const int32 NumMeshes = Data->Meshes.Num();
        for (int i = 0; i < NumMeshes; i++)
        {
            const AssimpMesh* Mesh = Data->Meshes[i];
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
            //DynMaterial->SetTextureParameterValue();
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
                    NewTexture->LossyCompressionAmount = TLCA_Highest;
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



            DynamicMesh->SetMaterial(i, DynMaterial);

            DynamicMesh->CreateMeshSection(i, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
        }
    }
}

// Called when the game starts or when spawned
void ARuntimeActorAsset::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARuntimeActorAsset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARuntimeActorAsset::UpdatePositionToGround(FVector& Position)
{
	constexpr float range = 10000.0f;
	FVector Start = Position + FVector(0.0f, 0.0f, range);
	FVector End = Position - FVector(0.0f, 0.0f, range);
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		Position = OutHit.ImpactPoint;
	}
}

void ARuntimeActorAsset::UpdatePositionToGroundLocal(FVector& Position)
{
	FTransform Transform = GetTransform();
	FVector WorldTransform = Transform.GetLocation() + Position;
	UpdatePositionToGround(WorldTransform);
	Position = Transform.InverseTransformPosition(WorldTransform);
}