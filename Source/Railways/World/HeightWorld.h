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
	
	void LinkAdjacency(AWorldTileDynamic* A, AWorldTileDynamic* B, uint32 ADirection);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(Server, Reliable)
	void TestForTile(int TileX, int TileY);
	//void TestForTile_Implementation(int TileX, int TileY);

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TestMesh;

	TMap<TPair<int, int>, AWorldTileDynamic*> Tiles;
	//UPROPERTY(Replicated)
	//TArray<AWorldTileDynamic*> Tiles;
	//AWorldTileDynamic* FindTile(int X, int Y);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		//DOREPLIFETIME(AHeightWorld, Tiles);
	}

	TArray<AWorldTileDynamic*> GetSortedTilesToPoint(FVector point);
	//URuntimeMeshComponent* Tile;
};
