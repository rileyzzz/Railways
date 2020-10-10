// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshActor.h"
#include "WorldTileProvider.h"
#include "Net/UnrealNetwork.h"
#include "WorldTileDynamic.generated.h"

USTRUCT()
struct FTerrainData
{
	GENERATED_BODY()

	//float* heightData;
	UPROPERTY()
	TArray<float> heightData;

	FTerrainData();
	~FTerrainData();

	//int16& operator[](int index);
	//bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	//bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	void SetHeightData(int x, int y, const float& height);
	void AddHeight(int x, int y, const float& height);
	float GetHeight(int x, int y);
};

//template<>
//struct TStructOpsTypeTraits<FTerrainData> : public TStructOpsTypeTraitsBase2<FTerrainData>
//{
//	enum
//	{
//		WithNetSerializer = true
//	};
//};


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

	//Transient,
	//UPROPERTY(Replicated, ReplicatedUsing = OnRep_heightData)
	//TArray<float> heightData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	//UPROPERTY(Replicated, ReplicatedUsing = OnRep_heightData)
	UPROPERTY()
	FTerrainData Terrain;
	//UFUNCTION(Server, Reliable)
	//void SetHeightData(int x, int y, int16 height);
	////void SetHeightData_Implementation(int x, int y, float height);

	////UFUNCTION(Server, Reliable)
	//void AddHeight(int x, int y, int16 height);
	////void AddHeight_Implementation(int x, int y, float height);

	//int16 GetHeight(int x, int y);

	//should happen on all clients

	//UFUNCTION(NetMulticast, Reliable)
	//void Build(int X, int Y);
	//void Build_Implementation(int X, int Y);

	//will modify replicated height data
	//needs to be reliable to sync correctly
	UFUNCTION(NetMulticast, Reliable)
	void TerrainInfluence(FVector Pos, float Direction, int Radius);
	void TerrainInfluence_Implementation(FVector Pos, float Direction, int Radius);

	UFUNCTION(NetMulticast, Reliable)
	void TerrainApproach(FVector Pos, float Height, float Strength, int Radius);
	void TerrainApproach_Implementation(FVector Pos, float Height, float Strength, int Radius);

	UPROPERTY(Replicated)
	int TileX;
	UPROPERTY(Replicated)
	int TileY;

	UFUNCTION()
	void OnRep_heightData();
	//void RefreshClientTile_Implementation();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		//DOREPLIFETIME(AWorldTileDynamic, Terrain);
		DOREPLIFETIME(AWorldTileDynamic, TileX);
		DOREPLIFETIME(AWorldTileDynamic, TileY);
	}

	AWorldTileDynamic();
	~AWorldTileDynamic();
};
