// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeActorAsset.h"
#include "SkeletalRuntimeActorAsset.generated.h"

//class FSkeletalMeshLODRenderData;

UCLASS()
class RAILWAYS_API ASkeletalRuntimeActorAsset : public ARuntimeActorAsset
{
	GENERATED_BODY()
private:

	USkeleton* Skeleton;

	UPROPERTY()
	USkeletalMesh* SkeletalMesh;

	UPROPERTY()
	USkeletalMeshComponent* MeshComponent;

	//UProceduralMeshComponent* MeshComponent;
	void GenerateSkeletalRenderMesh(TArray<FDynamicMeshVertex>& Vertices, TArray<FSoftSkinVertex>& SkinnedVertices, TArray<uint32>& Elements, FSkeletalMeshLODRenderData* LOD, FSkeletalMeshLODInfo& LODInfo, const AssimpNode& Node);
	//void BuildMeshNode(FSkeletalMeshLODRenderData& LOD, const AssimpNode& Node);
public:

	virtual void InitAsset() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ASkeletalRuntimeActorAsset();
	~ASkeletalRuntimeActorAsset();
};
