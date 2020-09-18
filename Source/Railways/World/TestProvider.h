// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "TestProvider.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API UTestProvider : public URuntimeMeshProvider
{
	GENERATED_BODY()
private:
	mutable FCriticalSection PropertySyncRoot;

		FVector BoxRadius = FVector(100.0f, 100.0f, 100.0f);

		UMaterialInterface* Material;
public:

		UMaterialInterface* GetBoxMaterial() const;
		void SetBoxMaterial(UMaterialInterface* InMaterial);


protected:
	void Initialize() override;
	FBoxSphereBounds GetBounds() override;
	bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	//FRuntimeMeshCollisionSettings GetCollisionSettings() override;
	//bool HasCollisionMesh() override;
	//bool GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData) override;
	bool IsThreadSafe() override;
};
