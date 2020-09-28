// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightWorld.h"

// Sets default values
AHeightWorld::AHeightWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile2")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile3")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile4")));
	//Tiles[0]->SetupAttachment(RootComponent);
	//Tiles[1]->SetupAttachment(RootComponent);
	//Tiles[2]->SetupAttachment(RootComponent);
	//Tiles[3]->SetupAttachment(RootComponent);
	
    
}

// Called when the game starts or when spawned
void AHeightWorld::BeginPlay()
{
	Super::BeginPlay();
	//Tiles[0]->Build(Material, 0, 0);
	//Tiles[1]->Build(Material, 0, 1);
	//Tiles[2]->Build(Material, 1, 0);
	//Tiles[3]->Build(Material, 1, 1);
}

void AHeightWorld::TestForTile(int TileX, int TileY)
{
	TPair<int, int> TilePos(TileX, TileY);
	if (!Tiles.Find(TilePos))
	{
		UWorldTileDynamic* NewTile = NewObject<UWorldTileDynamic>(this, NAME_None);
		NewTile->RegisterComponent();
		NewTile->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		NewTile->Build(Material, TileX, TileY);
		Tiles.Add(TilePos, NewTile);
		UE_LOG(LogTemp, Log, TEXT("Created tile at %i %i"), TileX, TileY);
	}
}

// Called every frame
void AHeightWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	APawn* LocalPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (LocalPawn)
	{
		FVector PlayerPos = LocalPawn->GetActorLocation() / ((float)WORLD_SIZE * WORLD_SCALE);
		int TileX = FMath::FloorToInt(PlayerPos.X);
		int TileY = FMath::FloorToInt(PlayerPos.Y);
		constexpr int BuildRadius = 4;
		for (int BuildX = TileX - BuildRadius; BuildX < TileX + BuildRadius; BuildX++)
		{
			for (int BuildY = TileY - BuildRadius; BuildY < TileY + BuildRadius; BuildY++)
			{
				TestForTile(BuildX, BuildY);
			}
		}
		
	}
}

TArray<UWorldTileDynamic*> AHeightWorld::GetSortedTilesToPoint(FVector point)
{
	TArray<UWorldTileDynamic*> Out;
	//TSortedMap<float, UWorldTileDynamic*> Distances;
	////Distances.Reserve(Tiles.Num());
	constexpr float HalfWorldSize = ((float)(WORLD_SIZE - 1) * WORLD_SCALE) / 2.0f;
	//for (auto& entry : Tiles)
	//{
	//	auto& tile = entry.Value;
	//	FTransform TilePos = tile->GetComponentTransform();
	//	TilePos.TransformPosition(FVector(HalfWorldSize, HalfWorldSize, 0.0f));
	//	TilePos.InverseTransformPosition(point);
	//	FVector RelativeLocation = TilePos.GetLocation();
	//	//Distances[RelativeLocation.Size()] = tile;

	//	//Distances.Add(TPairInitializer<float, UWorldTileDynamic*>(RelativeLocation.Size(), tile));
	//	Distances.Add(RelativeLocation.Size(), tile);
	//}
	//for (auto& dist : Distances)
	//	Out.Add(dist.Value);

	TSortedMap<float, UWorldTileDynamic*> Distances;
	for (auto& TileEntry : Tiles)
	{
		auto& Tile = TileEntry.Value;
		FVector TileLocation = Tile->GetComponentTransform().GetLocation() + FVector(HalfWorldSize, HalfWorldSize, 0.0f);
		float Distance = FVector::Dist(TileLocation, point);
		Distances.Add(Distance, Tile);
	}
	for (auto& dist : Distances)
		Out.Add(dist.Value);

	return Out;
}

