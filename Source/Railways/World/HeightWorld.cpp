// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightWorld.h"

// Sets default values
AHeightWorld::AHeightWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile")));
	Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile2")));
	Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile3")));
	Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile4")));
	Tiles[0]->SetupAttachment(RootComponent);
	Tiles[1]->SetupAttachment(RootComponent);
	Tiles[2]->SetupAttachment(RootComponent);
	Tiles[3]->SetupAttachment(RootComponent);
	
    
}

// Called when the game starts or when spawned
void AHeightWorld::BeginPlay()
{
	Super::BeginPlay();
	Tiles[0]->Build(Material, 0, 0);
	Tiles[1]->Build(Material, 0, 1);
	Tiles[2]->Build(Material, 1, 0);
	Tiles[3]->Build(Material, 1, 1);
}

// Called every frame
void AHeightWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<UWorldTileDynamic*> AHeightWorld::GetSortedTilesToPoint(FVector point)
{
	TArray<UWorldTileDynamic*> Out;
	TSortedMap<float, UWorldTileDynamic*> Distances;
	//Distances.Reserve(Tiles.Num());
	constexpr float HalfWorldSize = (float)(WORLD_SIZE - 1) * WORLD_SCALE / 2.0f;
	for (auto& tile : Tiles)
	{
		FTransform TilePos = tile->GetComponentTransform();
		TilePos.TransformPosition(FVector(HalfWorldSize, HalfWorldSize, 0.0f));
		TilePos.InverseTransformPosition(point);
		FVector RelativeLocation = TilePos.GetLocation();
		//Distances[RelativeLocation.Size()] = tile;

		//Distances.Add(TPairInitializer<float, UWorldTileDynamic*>(RelativeLocation.Size(), tile));
		Distances.Add(RelativeLocation.Size(), tile);
	}
	for (auto& dist : Distances)
		Out.Add(dist.Value);

	return Out;
}

