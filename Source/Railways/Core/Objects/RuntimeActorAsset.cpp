// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"

// Sets default values
ARuntimeActorAsset::ARuntimeActorAsset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARuntimeActorAsset::InitMesh()
{
	AssimpInterface = NewObject<UAssimpInterface>();
	auto Data = AssimpInterface->ImportFBX();

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

        DynamicMesh->CreateMeshSection(i, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
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