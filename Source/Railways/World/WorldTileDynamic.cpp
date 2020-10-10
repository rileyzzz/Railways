// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTileDynamic.h"
#include "DrawDebugHelpers.h"
//#include "Compression/lz4.h"
//#include "Core/Private/Compression/lz4.cpp"
#include <ctime>
//#include "Providers/RuntimeMeshProviderBox.h"
//#include "TestProvider.h"

//void AWorldTileDynamic::SetHeightData(int x, int y, int16 height)
//{
//    FScopeLock Lock(&PropertySyncRoot);
//    //f_heightData[x][y] = height;
//    heightData[x + y * WORLD_SIZE] = height;
//}
//
//void AWorldTileDynamic::AddHeight(int x, int y, int16 height)
//{
//    FScopeLock Lock(&PropertySyncRoot);
//    //f_heightData[x][y] += height;
//    heightData[x + y * WORLD_SIZE] += height;
//}
//
//int16 AWorldTileDynamic::GetHeight(int x, int y)
//{
//    if (x >= WORLD_SIZE) x = WORLD_SIZE - 1;
//    if (y >= WORLD_SIZE) y = WORLD_SIZE - 1;
//    if (x < 0) x = 0;
//    if (y < 0) y = 0;
//    //return f_heightData[x][y];
//    return heightData[x + y * WORLD_SIZE];
//}

//void AWorldTileDynamic::Build_Implementation(int X, int Y)
//{
//    UE_LOG(LogTemp, Warning, TEXT("building client tile at %i %i"), X, Y);
//    
//
//    //Provider = NewObject<UWorldTileProvider>(this, NAME_None);
//
//    //FVector Location = GetActorLocation();
//    //DrawDebugLine(GetWorld(), Location, Location + FVector(0.0f, 0.0f, 200.0f), FColor::Green, true);
//
//}

void AWorldTileDynamic::TerrainInfluence_Implementation(FVector Pos, float Direction, int Radius)
{
    int LocalRadius = Radius / 10.0f;
    FTransform transform = GetActorTransform();
    FVector InflPos = transform.InverseTransformPosition(Pos) / WORLD_SCALE;
    InflPos.Z = 0.0f;
    FVector Corner = InflPos - FVector(LocalRadius, LocalRadius, 0.0);

    for (int y = 0; y < LocalRadius * 2; y++)
    {
        float yalpha = ((float)y / (float)(LocalRadius * 2 - 1) - 0.5f) * 2.0f;
        int ypos = (FMath::FloorToInt(Corner.Y) + y);
        for (int x = 0; x < LocalRadius * 2; x++)
        {
            float xalpha = ((float)x / (float)(LocalRadius * 2 - 1) - 0.5f) * 2.0f;
            int xpos = (FMath::FloorToInt(Corner.X) + x);

            float dist = 1.0f - FMath::Sqrt(xalpha * xalpha + yalpha * yalpha);
            //UE_LOG(LogTemp, Warning, TEXT("checking bounds for %i %i"), xpos, ypos);
            if (dist > 0.0f && Provider->WithinBounds(xpos, ypos))
            {
                float Interpolated = (FMath::Sin(dist * PI - (PI / 2.0f)) + 1.0f) / 2.0f;
                Terrain.AddHeight(xpos, ypos, Interpolated * 10.0f * Direction); //dont change height  for world scale
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    //Provider->InvalidateMeshData();

    //Apply changes on server
    //OnRep_heightData();
    if(Provider) Provider->InvalidateMeshData();

    //Replicate all changes to clients
    //ForceNetUpdate();
}

void AWorldTileDynamic::TerrainApproach_Implementation(FVector Pos, float Height, float Strength, int Radius)
{
    int LocalRadius = Radius / 10.0f;
    FTransform transform = GetActorTransform();
    FVector InflPos = transform.InverseTransformPosition(Pos) / WORLD_SCALE;
    InflPos.Z = 0.0f;
    FVector Corner = InflPos - FVector(LocalRadius, LocalRadius, 0.0);

    for (int y = 0; y < LocalRadius * 2; y++)
    {
        float yalpha = ((float)y / (float)(LocalRadius * 2 - 1) - 0.5f) * 2.0f;
        int ypos = (FMath::FloorToInt(Corner.Y) + y);
        for (int x = 0; x < LocalRadius * 2; x++)
        {
            float xalpha = ((float)x / (float)(LocalRadius * 2 - 1) - 0.5f) * 2.0f;
            int xpos = (FMath::FloorToInt(Corner.X) + x);

            float dist = 1.0f - FMath::Sqrt(xalpha * xalpha + yalpha * yalpha);
            //UE_LOG(LogTemp, Warning, TEXT("checking bounds for %i %i"), xpos, ypos);
            if (dist > 0.0f && Provider->WithinBounds(xpos, ypos))
            {
                float StartHeight = Terrain.GetHeight(xpos, ypos);

                float Influence = (FMath::Sin(dist * PI - (PI / 2.0f)) + 1.0f) / 2.0f;
                //Interpolated *= Height;
                //Provider->AddHeight(xpos, ypos, (Interpolated * WORLD_SCALE * Strength) - StartHeight);
                Terrain.SetHeightData(xpos, ypos, FMath::Lerp(StartHeight, Height, Influence * Strength));
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    //Provider->InvalidateMeshData();

    //Apply changes on server
    if (Provider) Provider->InvalidateMeshData();

    //Replicate all changes to clients
    //ForceNetUpdate();
}

void AWorldTileDynamic::OnRep_heightData()
{
    Provider->InvalidateMeshData();
}

void AWorldTileDynamic::OnRep_Material()
{
    Provider->SetTileMaterial(Material);
}

AWorldTileDynamic::AWorldTileDynamic()
{
    if (HasAuthority()) SetReplicates(true);
    Provider = CreateDefaultSubobject<UWorldTileProvider>(TEXT("Provider"));

}

//void AWorldTileDynamic::PostInitProperties()
//{
//    Super::PostInitProperties();
//    
//}

// Called when the game starts or when spawned
void AWorldTileDynamic::BeginPlay()
{
    Super::BeginPlay();

    if (Provider)
    {
        Provider->SetTileParent(this);
        GetRuntimeMeshComponent()->Initialize(Provider);
    }
    
}

AWorldTileDynamic::~AWorldTileDynamic()
{
    //FMemory::Free(heightData);
}

FTerrainData::FTerrainData()
{
    //heightData = (float*)FMemory::Malloc(WORLD_SIZE * WORLD_SIZE * sizeof(float));

    heightData.Reserve(WORLD_SIZE * WORLD_SIZE);

    for (unsigned int x = 0; x < WORLD_SIZE; x++)
    {
        for (unsigned int y = 0; y < WORLD_SIZE; y++)
        {
            heightData.Add(0.0f);
            //heightData.Add((float)FMath::Rand() / (float) RAND_MAX * 10.0f);
            //SetHeightData(x, y, 0.0f);
        }
    }
}

FTerrainData::~FTerrainData()
{
    //if(heightData) FMemory::Free(heightData);
}

//int16& FTerrainData::operator[](int index)
//{
//    return heightData[index];
//}

bool FTerrainData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    heightData.Reserve(WORLD_SIZE * WORLD_SIZE); //ensure that array can fit required data

    constexpr int32 DataCapacity = WORLD_SIZE * WORLD_SIZE * sizeof(float);
    int32 compressSize = DataCapacity;
    float* compressData = (float*)FMemory::Malloc(DataCapacity);
    if (Ar.IsSaving())
    {
        FCompression::CompressMemory(NAME_LZ4, compressData, compressSize, heightData.GetData(), DataCapacity, COMPRESS_BiasMemory);
    }
    Ar << compressSize;
    for (int i = 0; i < compressSize; i++)
        Ar << compressData[i];

    if (Ar.IsLoading())
    {
        FCompression::UncompressMemory(NAME_LZ4, heightData.GetData(), DataCapacity, compressData, compressSize);
    }
    FMemory::Free(compressData);

    //for (unsigned int x = 0; x < WORLD_SIZE; x++)
    //{
    //    for (unsigned int y = 0; y < WORLD_SIZE; y++)
    //    {
    //        Ar << heightData[x + y * WORLD_SIZE];
    //    }
    //}

    bOutSuccess = true;
    return true;
}

//bool FTerrainData::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
//{
//    return FFastArraySerializer::FastArrayDeltaSerialize<float, FTerrainData>(heightData, DeltaParms, *this);
//}

void FTerrainData::SetHeightData(int x, int y, const float& height)
{
    heightData[x + y * WORLD_SIZE] = height;
}

void FTerrainData::AddHeight(int x, int y, const float& height)
{
    heightData[x + y * WORLD_SIZE] += height;
}

float FTerrainData::GetHeight(int x, int y)
{
    if (x >= WORLD_SIZE) x = WORLD_SIZE - 1;
    if (y >= WORLD_SIZE) y = WORLD_SIZE - 1;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    return heightData[x + y * WORLD_SIZE];
}
