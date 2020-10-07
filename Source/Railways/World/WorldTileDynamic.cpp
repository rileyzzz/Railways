// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTileDynamic.h"
#include "DrawDebugHelpers.h"
//#include "Providers/RuntimeMeshProviderBox.h"
//#include "TestProvider.h"

void UWorldTileDynamic::SetHeightData(int x, int y, float height)
{
    FScopeLock Lock(&PropertySyncRoot);
    //f_heightData[x][y] = height;
    heightData[x + y * WORLD_SIZE] = height;
}

void UWorldTileDynamic::AddHeight(int x, int y, float height)
{
    FScopeLock Lock(&PropertySyncRoot);
    //f_heightData[x][y] += height;
    heightData[x + y * WORLD_SIZE] += height;
}

float UWorldTileDynamic::GetHeight(int x, int y)
{
    if (x >= WORLD_SIZE) x = WORLD_SIZE - 1;
    if (y >= WORLD_SIZE) y = WORLD_SIZE - 1;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    //return f_heightData[x][y];
    return heightData[x + y * WORLD_SIZE];
}

void UWorldTileDynamic::Build(UMaterialInterface* Material, int X, int Y)
{
    SetRelativeLocation(FVector((float)X * (WORLD_SIZE - 1) * WORLD_SCALE, (float)Y * (WORLD_SIZE - 1) * WORLD_SCALE, 0.0f));

    Provider = NewObject<UWorldTileProvider>(this, NAME_None);

    FVector Location = GetComponentTransform().GetLocation();
    DrawDebugLine(GetWorld(), Location, Location + FVector(0.0f, 0.0f, 200.0f), FColor::Green, true);
    if (Provider)
    {
        Provider->SetTileMaterial(Material);
        Provider->SetTileParent(this);
        Initialize(Provider);
    }
}

void UWorldTileDynamic::TerrainInfluence_Implementation(FVector Pos, float Direction, int Radius)
{
    UE_LOG(LogTemp, Warning, TEXT("Influencing terrain!!!!!!"));
    int LocalRadius = Radius / 10.0f;
    FTransform transform = GetComponentTransform();
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
                AddHeight(xpos, ypos, Interpolated * 10.0f * Direction); //dont change height  for world scale
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    //Provider->InvalidateMeshData();
    RefreshClientTile(heightData);
}

void UWorldTileDynamic::TerrainApproach_Implementation(FVector Pos, float Height, float Strength, int Radius)
{
    int LocalRadius = Radius / 10.0f;
    FTransform transform = GetComponentToWorld();
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
                float StartHeight = GetHeight(xpos, ypos);

                float Influence = (FMath::Sin(dist * PI - (PI / 2.0f)) + 1.0f) / 2.0f;
                //Interpolated *= Height;
                //Provider->AddHeight(xpos, ypos, (Interpolated * WORLD_SCALE * Strength) - StartHeight);
                SetHeightData(xpos, ypos, FMath::Lerp(StartHeight, Height, Influence * Strength));
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    //Provider->InvalidateMeshData();
    RefreshClientTile(heightData);
}

void UWorldTileDynamic::RefreshClientTile_Implementation(const TArray<float>& inHeightData)
{
    heightData = inHeightData;
    Provider->InvalidateMeshData();
}

UWorldTileDynamic::UWorldTileDynamic()
{
    //heightData = (float*)FMemory::Malloc(WORLD_SIZE * WORLD_SIZE * sizeof(float));
    //i really don't like using a TArray because it's not necessary but it wont replicate otherwise
    heightData.Reserve(WORLD_SIZE * WORLD_SIZE);
    for (unsigned int x = 0; x < WORLD_SIZE; x++)
    {
        for (unsigned int y = 0; y < WORLD_SIZE; y++)
        {
            heightData.Add(0.0f);
            //SetHeightData(x, y, 0.0f);
        }
    }
}

UWorldTileDynamic::~UWorldTileDynamic()
{
    //FMemory::Free(heightData);
}
