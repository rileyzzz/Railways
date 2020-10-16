// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//USTRUCT(BlueprintType)
//struct FProcSkeletalMeshTangent
//{
//	GENERATED_BODY()
//public:
//
//	/** Direction of X tangent for this vertex */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
//		FVector TangentX;
//
//	/** Bool that indicates whether we should flip the Y tangent when we compute it using cross product */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
//		bool bFlipTangentY;
//
//	FProcSkeletalMeshTangent()
//		: TangentX(1.f, 0.f, 0.f)
//		, bFlipTangentY(false)
//	{}
//
//	FProcSkeletalMeshTangent(float X, float Y, float Z)
//		: TangentX(X, Y, Z)
//		, bFlipTangentY(false)
//	{}
//
//	FProcSkeletalMeshTangent(FVector InTangentX, bool bInFlipTangentY)
//		: TangentX(InTangentX)
//		, bFlipTangentY(bInFlipTangentY)
//	{}
//};
//
//USTRUCT(BlueprintType)
//struct FProcSkeletalMeshVertex
//{
//	GENERATED_BODY()
//public:
//
//	/** Vertex position */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
//		FVector Position;
//
//	/** Vertex normal */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
//		FVector Normal;
//
//	/** Vertex tangent */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
//		FProcSkeletalMeshTangent Tangent;
//
//	/** Vertex color */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
//		FColor Color;
//
//	/** Vertex texture co-ordinate */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
//		FVector2D UV0;
//
//	//why tf are people trying to use UDIMs or whatever and skinning never crossed their mind
//
//	//Bone stuff
//	FBoneIndexType	InfluenceBones[MAX_TOTAL_INFLUENCES];
//	uint8			InfluenceWeights[MAX_TOTAL_INFLUENCES];
//
//	/** If this vert is rigidly weighted to a bone, return true and the bone index. Otherwise return false. */
//	bool GetRigidWeightBone(FBoneIndexType& OutBoneIndex) const;
//
//	/** Returns the maximum weight of any bone that influences this vertex. */
//	uint8 GetMaximumWeight() const;
//
//	FProcSkeletalMeshVertex()
//		: Position(0.f, 0.f, 0.f)
//		, Normal(0.f, 0.f, 1.f)
//		, Tangent(FVector(1.f, 0.f, 0.f), false)
//		, Color(255, 255, 255)
//		, UV0(0.f, 0.f)
//	{}
//};
