// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "WorldTileProvider.h"
#include "WorldTileDynamic.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API UWorldTileDynamic : public URuntimeMeshComponent
{
	GENERATED_BODY()
private:
	UWorldTileProvider* Provider;
public:
	void Build(UMaterialInterface* Material, int X, int Y);
	void TerrainInfluence(FVector Pos, float Direction, int Radius);
	void TerrainApproach(FVector Pos, float Height, float Strength, int Radius);
};
