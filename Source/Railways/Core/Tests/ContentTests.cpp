// Fill out your copyright notice in the Description page of Project Settings.


#include "ContentTests.h"

// Sets default values
AContentTests::AContentTests()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Skeleton = CreateDefaultSubobject<UProceduralSkeletalMeshComponent>(TEXT("skeleton"));
	Skeleton->SetupAttachment(RootComponent);
	Statictest = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("staticthing"));
	Statictest->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AContentTests::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Beginning Assimp"));
	Interface = NewObject<UAssimpInterface>();
	auto Data = Interface->ImportFBX();
	//Interface->BuildComponent(Skeleton, test);

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
            Positions[vert] = VertData.Location;
            Normals[vert] = VertData.Normal;
            //Normals[vert] = FVector(0.0f, 1.0f, 0.0f);
            //Normals[vert] = (VertData.Normal * FVector(0.0f, -1.0f, 0.0f)) + VertData.Location;
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

        Statictest->CreateMeshSection(i, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
    }
}

// Called every frame
void AContentTests::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

