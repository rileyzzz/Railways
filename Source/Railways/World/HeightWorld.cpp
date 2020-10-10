// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightWorld.h"

// Sets default values
AHeightWorld::AHeightWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetMobility(EComponentMobility::Type::Static);
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile2")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile3")));
	//Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile4")));
	//Tiles[0]->SetupAttachment(RootComponent);
	//Tiles[1]->SetupAttachment(RootComponent);
	//Tiles[2]->SetupAttachment(RootComponent);
	//Tiles[3]->SetupAttachment(RootComponent);

	if(HasAuthority()) SetReplicates(true);
}

// Called when the game starts or when spawned
void AHeightWorld::BeginPlay()
{
	Super::BeginPlay();

	//APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	//if(Controller) SetOwner(Controller);

	//Tiles[0]->Build(Material, 0, 0);
	//Tiles[1]->Build(Material, 0, 1);
	//Tiles[2]->Build(Material, 1, 0);
	//Tiles[3]->Build(Material, 1, 1);
}

void AHeightWorld::TestForTile(int TileX, int TileY)
{
	TPair<int, int> TilePos(TileX, TileY);
	if (!Tiles.Find(TilePos))//Tiles.Find(TilePos)
	{
		//AWorldTileDynamic* NewTile = NewObject<AWorldTileDynamic>(this, NAME_None);
		//NewTile->RegisterComponent();
		//NewTile->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		//NewTile->SetNetAddressable();
		//NewTile->SetIsReplicated(true);
		//NewTile->Build(Material, TileX, TileY);
		FActorSpawnParameters SpawnParams;
		//SpawnParams.Name = NAME_None;

		FVector TileLocation((float)TileX * (WORLD_SIZE - 1) * WORLD_SCALE, (float)TileY * (WORLD_SIZE - 1) * WORLD_SCALE, 0.0f);
		FRotator TileRotation;
		AWorldTileDynamic* NewTile = GetWorld()->SpawnActor<AWorldTileDynamic>(AWorldTileDynamic::StaticClass(), TileLocation, TileRotation, SpawnParams);
		NewTile->SetReplicates(true);
		NewTile->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		NewTile->bNetLoadOnClient = true;
		//NewTile->Build(TileX, TileY); //Material

		NewTile->TileX = TileX;
		NewTile->TileY = TileY;
		NewTile->Material = Material;
		NewTile->OnRep_Material(); //change material on the server
		NewTile->ForceNetUpdate(); //force update material on all clients

		//NewTile->SetMobility(EComponentMobility::Type::Movable);

		Tiles.Add(TilePos, NewTile);
		//Tiles.Add(NewTile);
		UE_LOG(LogTemp, Log, TEXT("Created tile at %i %i"), TileX, TileY);
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("SERVER EXEC"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CLIENT EXEC"));
		}
	}
}

// Called every frame
void AHeightWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	//if (Controller)
	//{
	//	APawn* LocalPawn = Controller->GetPawn();
	//	if (LocalPawn)
	//	{
	//		FVector PlayerPos = LocalPawn->GetActorLocation() / ((float)WORLD_SIZE * WORLD_SCALE);
	//		int TileX = FMath::FloorToInt(PlayerPos.X);
	//		int TileY = FMath::FloorToInt(PlayerPos.Y);
	//		constexpr int BuildRadius = 4;
	//		for (int BuildX = TileX - BuildRadius; BuildX < TileX + BuildRadius; BuildX++)
	//		{
	//			for (int BuildY = TileY - BuildRadius; BuildY < TileY + BuildRadius; BuildY++)
	//			{
	//				TestForTile(BuildX, BuildY);
	//			}
	//		}

	//	}
	//}

	//need to execute tile creation on server
	//if (HasAuthority())
	//{
	//	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	//	{
	//		APlayerController* Controller = it->Get();
	//		APawn* LocalPawn = Controller->GetPawn();
	//		if (LocalPawn)
	//		{
	//			FVector PlayerPos = LocalPawn->GetActorLocation() / ((float)WORLD_SIZE * WORLD_SCALE);
	//			int TileX = FMath::FloorToInt(PlayerPos.X);
	//			int TileY = FMath::FloorToInt(PlayerPos.Y);
	//			constexpr int BuildRadius = 4;
	//			for (int BuildX = TileX - BuildRadius; BuildX < TileX + BuildRadius; BuildX++)
	//			{
	//				for (int BuildY = TileY - BuildRadius; BuildY < TileY + BuildRadius; BuildY++)
	//				{
	//					TestForTile(BuildX, BuildY);
	//				}
	//			}

	//		}
	//	}
	//}
}

//AWorldTileDynamic* AHeightWorld::FindTile(int X, int Y)
//{
//	for (const auto& Tile : Tiles)
//	{
//		if (Tile->TileX == X && Tile->TileY == Y)
//			return Tile;
//	}
//	return nullptr;
//}

TArray<AWorldTileDynamic*> AHeightWorld::GetSortedTilesToPoint(FVector point)
{
	TArray<AWorldTileDynamic*> Out;
	constexpr float HalfWorldSize = ((float)(WORLD_SIZE - 1) * WORLD_SCALE) / 2.0f;

	TSortedMap<float, AWorldTileDynamic*> Distances;
	for (auto& TileEntry : Tiles) //TileEntry
	{
		auto& Tile = TileEntry.Value;
		FVector TileLocation = Tile->GetActorTransform().GetLocation() + FVector(HalfWorldSize, HalfWorldSize, 0.0f);
		float Distance = FVector::Dist(TileLocation, point);
		Distances.Add(Distance, Tile);
	}
	for (auto& dist : Distances)
		Out.Add(dist.Value);

	return Out;
}

