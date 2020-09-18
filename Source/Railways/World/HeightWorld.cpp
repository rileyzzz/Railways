// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightWorld.h"

// Sets default values
AHeightWorld::AHeightWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Tiles.Add(CreateDefaultSubobject<UWorldTileDynamic>(TEXT("Tile")));
	Tiles[0]->SetupAttachment(RootComponent);
	
    
}

// Called when the game starts or when spawned
void AHeightWorld::BeginPlay()
{
	Super::BeginPlay();
	Tiles[0]->Build(Material);
}

// Called every frame
void AHeightWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

