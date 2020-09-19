// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define WORLD_SIZE 100
#define WORLD_SCALE 40.0f
#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "WorldTileProvider.generated.h"

/**
 * 
 */


UCLASS(HideCategories = Object, BlueprintType)
class RAILWAYS_API UWorldTileProvider : public URuntimeMeshProvider
{
	GENERATED_BODY()

private:
	mutable FCriticalSection PropertySyncRoot;

	UMaterialInterface* Material;

	float** f_heightData;
	
public:

	UMaterialInterface* GetTileMaterial() const;
	void SetTileMaterial(UMaterialInterface* InMaterial);
	void SetHeightData(int x, int y, float height);
	void AddHeight(int x, int y, float height);
	float GetHeight(int x, int y);
	bool WithinBounds(int x, int y);
	void InvalidateMeshData();
	//UWorldTileProvider();

protected:
	void Initialize() override;
	FBoxSphereBounds GetBounds() override;
	bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	FRuntimeMeshCollisionSettings GetCollisionSettings() override;
	bool HasCollisionMesh() override;
	bool GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData) override;
	bool IsThreadSafe() override;
};
