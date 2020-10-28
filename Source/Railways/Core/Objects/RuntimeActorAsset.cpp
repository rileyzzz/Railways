// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#include "../RailwaysGameInstance.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Helpers/stb_image.h"

void MaterialLoadAsyncTask::DoWork()
{
    for (int32 i = 0; i < Materials.Num(); i++)
    {
        RailwaysMaterial& Material = Materials[i];
        UE_LOG(LogTemp, Display, TEXT("loading material with %i textures"), Material.Textures.Num());
        //create material
        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(ParentActor->PBRMaterial, ParentActor);

        TMap<UTexture2D*, FName> TextureMap;
        //create textures
        for (const auto& Texture : Material.Textures)
        {
            bool SRGB = (Texture.Type == TextureType::Diffuse);
            //UE_LOG(LogTemp, Warning, TEXT("texture %s"), *(MeshContent.DataPath + Texture.Path));
            if (!ParentActor) break;
            UTexture2D* NewTexture = ParentActor->LoadTextureFile(Part->MeshContent.DataPath + Texture.Path, SRGB);

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

                TextureMap.Add(NewTexture, TextureTarget);
            }
        }

        if (!ParentActor) break;
        AsyncTask(ENamedThreads::GameThread, [this, i, DynMaterial, TextureMap]() {
            for (const auto& tex : TextureMap)
            {
                DynMaterial->SetTextureParameterValue(tex.Value, tex.Key);
            }
            ParentActor->MaterialInstances.Add(DynMaterial);
            ParentActor->MaterialInitCallback(i);
            });
    }

    UE_LOG(LogTemp, Display, TEXT("Finished loading textures."));
}

UTexture2D* ARuntimeActorAsset::LoadTextureFile(FString Path, bool SRGB)
{
    UTexture2D* NewTexture = nullptr;
    if (TextureCache.Contains(Path))
    {
        NewTexture = TextureCache[Path];
    }
    else
    {
        //NewTexture = FImageUtils::ImportFileAsTexture2D(Path);
        //if (NewTexture)
        //{
        //    NewTexture->SRGB = SRGB;
        //    //NewTexture->UpdateResource();
        //    TextureCache.Add(Path, NewTexture);
        //}
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
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load texture %s"), *Path);
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

void ARuntimeActorAsset::BeginDestroy()
{
    Super::BeginDestroy();
    
    //UE_LOG(LogTemp, Warning, TEXT("Begin destroy"));
    //if (MaterialLoadTask)
    //{
    //    //MaterialLoadTask->Cancel();
    //    if (!MaterialLoadTask->IsDone())
    //    {
    //        MaterialLoadTask->Cancel();
    //    }

    //    //if (!MaterialLoadTask->IsDone()) MaterialLoadTask->GetTask().Abandon();
    //    //delete MaterialLoadTask;
    //}
}

ARuntimeActorAsset::~ARuntimeActorAsset()
{

}

void ARuntimeActorAsset::InitMaterialsAsync()
{
    for(auto& Part : MeshParts)
        (new FAutoDeleteAsyncTask<MaterialLoadAsyncTask>(Part.MeshContent.MeshData->Materials, this, &Part))->StartBackgroundTask();

    //MaterialLoadTask = new FAsyncTask<MaterialLoadAsyncTask>(MeshContent.MeshData->Materials, this);
    //MaterialLoadTask->StartBackgroundTask();
}

void ARuntimeActorAsset::InitAsset()
{
    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    //if (GameInstance) MeshContent = GameInstance->AssimpInterface->ImportFBX(false);
    //MeshContent.LoadMesh(TEXT("E:/Users/riley_000/Documents/Unreal Projects/Railways/Plugins/ContentSystem/Content/Samples/pb15main/pb15_lod0.rmsh"));
    if (GameInstance)
    {
        UContentSystemInterface* Interface = GameInstance->ContentInterface;
        ContentInfo = Interface->RetrieveContentData(TEXT("00000000"));
        UE_LOG(LogTemp, Display, TEXT("Content directory %s"), *ContentInfo.ContentDir);

        FMeshPart NewPart;
        NewPart.MeshContent.LoadMesh(FPaths::Combine(ContentInfo.ContentDir, TEXT("pb15_lod0.rmsh")));
        //MeshContent.LoadMesh(FPaths::Combine(ContentInfo.ContentDir, TEXT("pb15_lod0.rmsh")));
        MeshParts.Add(NewPart);

        InitMaterialsAsync();
    }
    UE_LOG(LogTemp, Warning, TEXT("Mesh loaded"));
    
    

    //initialize materials
    //for (const auto& Material : MeshContent.MeshData->Materials)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("loading material with %i textures"), Material.Textures.Num());
    //    //create material
    //    UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PBRMaterial, this);

    //    //create textures
    //    for (const auto& Texture : Material.Textures)
    //    {
    //        bool SRGB = (Texture.Type == TextureType::Diffuse);
    //        UE_LOG(LogTemp, Warning, TEXT("texture %s"), *(MeshContent.DataPath + Texture.Path));
    //        UTexture2D* NewTexture = LoadTextureFile(MeshContent.DataPath + Texture.Path, SRGB);

    //        if (NewTexture)
    //        {
    //            FName TextureTarget;
    //            switch (Texture.Type)
    //            {
    //            default:
    //            case TextureType::Diffuse:
    //                TextureTarget = FName(TEXT("BaseColor"));
    //                break;
    //            case TextureType::Normal:
    //                TextureTarget = FName(TEXT("Normal"));
    //                break;
    //            case TextureType::Parameter:
    //                TextureTarget = FName(TEXT("Parameter"));
    //                break;
    //            }

    //            DynMaterial->SetTextureParameterValue(TextureTarget, NewTexture);
    //        }
    //    }

    //    MaterialInstances.Add(DynMaterial);
    //}
}

void ARuntimeActorAsset::MaterialInitCallback(int32 index)
{
    UE_LOG(LogTemp, Error, TEXT("Virtual material init callback failed!"));
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
