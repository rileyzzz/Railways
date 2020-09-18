// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTile.h"
//#include "Providers/RuntimeMeshProviderStatic.h"
#include "WorldTileProvider.h"

void UWorldTile::Build(UMaterialInterface* Material)
{
    UWorldTileProvider* Provider = NewObject<UWorldTileProvider>(this, TEXT("RuntimeMeshProvider"));
    //URuntimeMeshProviderPlane* Provider = NewObject<URuntimeMeshProviderPlane>(this, TEXT("RuntimeMeshProvider-Box"));
    if (Provider)
    {
        // The static provider should initialize before we use it
        //Initialize(Provider);
        //Provider->SetPlaneRadius(FVector(100, 100, 100));
        //Provider->SetPlaneMaterial(Material);
        Provider->SetTileMaterial(Material);
        //Initialize(Provider);
        GetRuntimeMesh()->Initialize(Provider);
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
}