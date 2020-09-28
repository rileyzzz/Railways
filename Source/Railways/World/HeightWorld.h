// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldTileDynamic.h"
#include "HeightWorld.generated.h"

UCLASS()
class RAILWAYS_API AHeightWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeightWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void TestForTile(int TileX, int TileY);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	//TArray<UWorldTileDynamic*> Tiles;
	TMap<TPair<int, int>, UWorldTileDynamic*> Tiles;

	TArray<UWorldTileDynamic*> GetSortedTilesToPoint(FVector point);
	//URuntimeMeshComponent* Tile;
};
