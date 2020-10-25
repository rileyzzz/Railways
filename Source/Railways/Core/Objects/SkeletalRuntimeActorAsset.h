// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeActorAsset.h"
#include "Components/PoseableMeshComponent.h"
#include "SkeletalRuntimeActorAsset.generated.h"

//class FSkeletalMeshLODRenderData;
struct FSoftSkinVertex;

#define USE_EDITOR_RENDER 0

UCLASS()
class RAILWAYS_API ASkeletalRuntimeActorAsset : public ARuntimeActorAsset
{
	GENERATED_BODY()
private:

	//USkeleton* Skeleton;

	UPROPERTY()
	USkeletalMesh* SkeletalMesh;

	UPROPERTY()
	USkeletalMeshComponent* MeshComponent;
	//UPoseableMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* TestAnim;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	UAnimSequence* Animation;
	//UProceduralMeshComponent* MeshComponent; TArray<FDynamicMeshVertex>& Vertices, 

#if USE_EDITOR_RENDER
	void GenerateSkeletalRenderMesh(TArray<FSoftSkinVertex>& Vertices, TArray<uint32>& Elements, FSkeletalMeshLODModel* LOD, FSkeletalMeshLODInfo& LODInfo, const RailwaysNode& Node, const int BoneCount);
#else
	void GenerateSkeletalRenderMesh(TArray<FSoftSkinVertex>& Vertices, TArray<uint32>& Elements, FSkeletalMeshLODRenderData* LOD, FSkeletalMeshLODInfo& LODInfo, const RailwaysNode& Node, const int BoneCount);
#endif

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
