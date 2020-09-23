// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProceduralSkeletalMeshComponent.generated.h"

/**
 * 
 */

struct SkeletalVert
{
	FVector Location;
	FVector Normal;
	FVector Tangent;
	FVector2D TexCoords;

	SkeletalVert(FVector InLocation, FVector InNormal, FVector InTangent, FVector2D InTexCoords) : Location(InLocation), Normal(InNormal), Tangent(InTangent), TexCoords(InTexCoords) { }
};

struct SkeletalMeshData
{
	TArray<SkeletalVert> Vertices;
	TArray<uint32> Elements;
};

UCLASS()
class PROCEDURALSKELETALMESH_API UProceduralSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	//UActorComponent overrides
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SendRenderDynamicData_Concurrent() override;
	virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;

	//USceneComponent overrides
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	//UPrimitiveComponent overrides
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	//UMeshComponent overrides
	virtual int32 GetNumMaterials() const override;

	void LoadMeshData(TArray<SkeletalMeshData>& data);

	friend class FProceduralSkeletalMeshSceneProxy;
};
