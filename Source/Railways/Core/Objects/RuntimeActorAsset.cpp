// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#include "../RailwaysGameInstance.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Helpers/stb_image.h"

UTexture2D* ARuntimeActorAsset::LoadTextureFile(FString Path, bool SRGB)
{
    UTexture2D* NewTexture = nullptr;
    if (TextureCache.Contains(Path))
    {
        NewTexture = TextureCache[Path];
    }
    else
    {
        int width, height, nrComponents;
        unsigned char* data = stbi_load(TCHAR_TO_UTF8(*Path), &width, &height, &nrComponents, 4);
        if (data)
        {
            FName TextureName = MakeUniqueObjectName(this, UTexture2D::StaticClass());
            NewTexture = NewObject<UTexture2D>(this, TextureName, RF_Transient);

            EPixelFormat format = PF_R8G8B8A8;
            NewTexture->PlatformData = new FTexturePlatformData();
            NewTexture->PlatformData->SizeX = width;
            NewTexture->PlatformData->SizeY = height;
            NewTexture->PlatformData->PixelFormat = format;

            NewTexture->SRGB = SRGB;

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

            TextureCache.Add(Path, NewTexture);
        }
    }
    return NewTexture;
}

// Sets default values
ARuntimeActorAsset::ARuntimeActorAsset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    //DynamicMesh = CreateDefaultSubobject<UProceduralSkeletalMeshComponent>(TEXT("Mesh"));
    //DynamicMesh->SetupAttachment(RootComponent);
}

void ARuntimeActorAsset::InitAsset()
{
    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    //if (GameInstance) MeshContent = GameInstance->AssimpInterface->ImportFBX(false);
    MeshContent.LoadMesh(TEXT("E:/Users/riley_000/Documents/Unreal Projects/Railways/Plugins/ContentSystem/Content/Samples/other/neville/test4.rmsh"));
    UE_LOG(LogTemp, Warning, TEXT("Mesh loaded"));

    //initialize materials
    for (const auto& Material : MeshContent.MeshData->Materials)
    {
        UE_LOG(LogTemp, Warning, TEXT("loading material with %i textures"), Material.Textures.Num());
        //create material
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PBRMaterial, this);

        //create textures
        for (const auto& Texture : Material.Textures)
        {
            bool SRGB = (Texture.Type == TextureType::Diffuse);
            UE_LOG(LogTemp, Warning, TEXT("texture %s"), *(MeshContent.DataPath + Texture.Path));
            UTexture2D* NewTexture = LoadTextureFile(MeshContent.DataPath + Texture.Path, SRGB);

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
}

// Called when the game starts or when spawned
void ARuntimeActorAsset::BeginPlay()
{
	Super::BeginPlay();
    //URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    //if (GameInstance)
    //{
    //    MeshData = GameInstance->AssimpInterface->ImportFBX(true);
    //    InitAsset();
    //}
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