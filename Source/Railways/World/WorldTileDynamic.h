// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshActor.h"
#include "WorldTileProvider.h"
#include "Net/UnrealNetwork.h"
#include "WorldTileDynamic.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API AWorldTileDynamic : public ARuntimeMeshActor
{
	GENERATED_BODY()
private:
	mutable FCriticalSection PropertySyncRoot;

	UWorldTileProvider* Provider;

	
	//float* heightData;
	//UPROPERTY(Replicated)
	//float heightData[WORLD_SIZE * WORLD_SIZE];

	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_heightData)
	TArray<float> heightData;
public:

	//UFUNCTION(Server, Reliable)
	void SetHeightData(int x, int y, float height);
	//void SetHeightData_Implementation(int x, int y, float height);

	//UFUNCTION(Server, Reliable)
	void AddHeight(int x, int y, float height);
	//void AddHeight_Implementation(int x, int y, float height);

	float GetHeight(int x, int y);

	//should happen on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Build(int X, int Y);
	void Build_Implementation(int X, int Y);

	//will modify replicated height data
	//UFUNCTION(Server, Reliable)
	void TerrainInfluence(FVector Pos, float Direction, int Radius);
	//void TerrainInfluence_Implementation(FVector Pos, float Direction, int Radius);

	//UFUNCTION(Server, Reliable)
	void TerrainApproach(FVector Pos, float Height, float Strength, int Radius);
	//void TerrainApproach_Implementation(FVector Pos, float Height, float Strength, int Radius);

	int TileX;
	int TileY;
	//UFUNCTION(NetMulticast, Reliable)
	UFUNCTION()
	void OnRep_heightData();
	//void RefreshClientTile_Implementation();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		DOREPLIFETIME(AWorldTileDynamic, heightData);
	}

	AWorldTileDynamic();
	~AWorldTileDynamic();
};
