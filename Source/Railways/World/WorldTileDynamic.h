// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "WorldTileProvider.h"
#include "Net/UnrealNetwork.h"
#include "WorldTileDynamic.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API UWorldTileDynamic : public URuntimeMeshComponent
{
	GENERATED_BODY()
private:
	mutable FCriticalSection PropertySyncRoot;

	UWorldTileProvider* Provider;

	
	//float* heightData;
	//UPROPERTY(Replicated)
	//float heightData[WORLD_SIZE * WORLD_SIZE];

	//UPROPERTY(Replicated)
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
	void Build(UMaterialInterface* Material, int X, int Y);

	UFUNCTION(Server, Reliable)
	void TerrainInfluence(FVector Pos, float Direction, int Radius);
	void TerrainInfluence_Implementation(FVector Pos, float Direction, int Radius);

	UFUNCTION(Server, Reliable)
	void TerrainApproach(FVector Pos, float Height, float Strength, int Radius);
	void TerrainApproach_Implementation(FVector Pos, float Height, float Strength, int Radius);

	UFUNCTION(NetMulticast, Reliable)
	void RefreshClientTile(const TArray<float>& inHeightData);
	void RefreshClientTile_Implementation(const TArray<float>& inHeightData);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		//DOREPLIFETIME(UWorldTileDynamic, heightData);
	}

	UWorldTileDynamic();
	~UWorldTileDynamic();
};
