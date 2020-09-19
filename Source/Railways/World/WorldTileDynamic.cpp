// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTileDynamic.h"
//#include "Providers/RuntimeMeshProviderBox.h"
//#include "TestProvider.h"

void UWorldTileDynamic::Build(UMaterialInterface* Material, int X, int Y)
{
    SetRelativeLocation(FVector((float)X * (WORLD_SIZE - 1) * WORLD_SCALE, (float)Y * (WORLD_SIZE - 1) * WORLD_SCALE, 0.0f));

    Provider = NewObject<UWorldTileProvider>(this, TEXT("RuntimeMeshProvider"));
    //URuntimeMeshProviderPlane* Provider = NewObject<URuntimeMeshProviderPlane>(this, TEXT("RuntimeMeshProvider-Box"));
    
    
    if (Provider)
    {
        UE_LOG(LogTemp, Warning, TEXT("creating tile"));
        // The static provider should initialize before we use it
        //Initialize(Provider);
        //Provider->SetPlaneRadius(FVector(100, 100, 100));
        //Provider->SetPlaneMaterial(Material);
        Provider->SetTileMaterial(Material);
        Initialize(Provider);

        //for (unsigned int i = 0; i < WORLD_SIZE; i++)
        //    for (unsigned int j = 0; j < WORLD_SIZE; j++)
        //        Provider->SetHeightData(i, j, (float)FMath::Sin(i / 10.0f) * 100.0f);


        //GetRuntimeMesh()->Initialize(Provider);
        //Provider->SetupMaterialSlot(0, TEXT("TriMat"), Material);


        // This creates 3 positions for a triangle
        //TArray<FVector> Positions{ FVector(0, -50, 0), FVector(0, 0, 100), FVector(0, 50, 0) };

        //// This creates 3 vertex colors
        //TArray<FColor> Colors{ FColor::Blue, FColor::Red, FColor::Green };

        //// This indexes our simple triangle
        //TArray<int32> Triangles = { 0, 1, 2 };

        //TArray<FVector> EmptyNormals;
        //TArray<FVector2D> EmptyTexCoords;
        //TArray<FRuntimeMeshTangent> EmptyTangents;
        //Provider->CreateSectionFromComponents(0, 0, 0, Positions, Triangles, EmptyNormals, EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Frequent, true);
    }

    //UTestProvider* BoxProvider = NewObject<UTestProvider>(this, TEXT("RuntimeMeshProvider-Box"));
    //if (BoxProvider)
    //{
    //    //BoxProvider->SetBoxRadius(FVector(100, 100, 100));
    //    BoxProvider->SetBoxMaterial(Material);

    //    Initialize(BoxProvider);
    //}
}

void UWorldTileDynamic::TerrainInfluence(FVector Pos, float Direction, int Radius)
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
                float Interpolated = (FMath::Sin(dist * PI - (PI / 2.0f)) + 1.0f) / 2.0f;
                Provider->AddHeight(xpos, ypos, Interpolated * 10.0f * Direction); //dont change height  for world scale
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    Provider->InvalidateMeshData();
}

void UWorldTileDynamic::TerrainApproach(FVector Pos, float Height, float Strength, int Radius)
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
                float StartHeight = Provider->GetHeight(xpos, ypos);

                float Influence = (FMath::Sin(dist * PI - (PI / 2.0f)) + 1.0f) / 2.0f;
                //Interpolated *= Height;
                //Provider->AddHeight(xpos, ypos, (Interpolated * WORLD_SCALE * Strength) - StartHeight);
                Provider->SetHeightData(xpos, ypos, FMath::Lerp(StartHeight, Height, Influence * Strength));
            }
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("influence at %f %f %f"), InflPos.X, InflPos.Y, InflPos.Z);

    Provider->InvalidateMeshData();
}
