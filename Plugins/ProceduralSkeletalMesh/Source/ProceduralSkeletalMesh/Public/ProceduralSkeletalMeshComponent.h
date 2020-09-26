// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProceduralSkeletalMeshComponent.generated.h"

USTRUCT(BlueprintType)
struct FProcSkeletalMeshTangent
{
	GENERATED_BODY()
public:

	/** Direction of X tangent for this vertex */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
		FVector TangentX;

	/** Bool that indicates whether we should flip the Y tangent when we compute it using cross product */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
		bool bFlipTangentY;

	FProcSkeletalMeshTangent()
		: TangentX(1.f, 0.f, 0.f)
		, bFlipTangentY(false)
	{}

	FProcSkeletalMeshTangent(float X, float Y, float Z)
		: TangentX(X, Y, Z)
		, bFlipTangentY(false)
	{}

	FProcSkeletalMeshTangent(FVector InTangentX, bool bInFlipTangentY)
		: TangentX(InTangentX)
		, bFlipTangentY(bInFlipTangentY)
	{}
};

USTRUCT(BlueprintType)
struct FProcSkeletalMeshVertex
{
	GENERATED_BODY()
public:

	/** Vertex position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
		FVector Position;

	/** Vertex normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
		FVector Normal;

	/** Vertex tangent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
		FProcSkeletalMeshTangent Tangent;

	/** Vertex color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
		FColor Color;

	/** Vertex texture co-ordinate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
		FVector2D UV0;

	//why tf are people trying to use UDIMs or whatever and skinning never crossed their mind

	FProcSkeletalMeshVertex()
		: Position(0.f, 0.f, 0.f)
		, Normal(0.f, 0.f, 1.f)
		, Tangent(FVector(1.f, 0.f, 0.f), false)
		, Color(255, 255, 255)
		, UV0(0.f, 0.f)
	{}
};

USTRUCT()
struct FProcSkeletalMeshSection
{
	GENERATED_BODY()
public:

	/** Vertex buffer for this section */
	UPROPERTY()
	TArray<FProcSkeletalMeshVertex> ProcVertexBuffer;

	/** Index buffer for this section */
	UPROPERTY()
	TArray<uint32> ProcIndexBuffer;
	/** Local bounding box of section */
	UPROPERTY()
	FBox SectionLocalBox;

	/** Should we build collision data for triangles in this section */
	//UPROPERTY()
	//bool bEnableCollision;

	/** Should we display this section */
	UPROPERTY()
	bool bSectionVisible;

	FProcSkeletalMeshSection()
		: SectionLocalBox(ForceInit)
		//, bEnableCollision(false)
		, bSectionVisible(true)
	{}

	/** Reset this section, clear all mesh info. */
	void Reset()
	{
		ProcVertexBuffer.Empty();
		ProcIndexBuffer.Empty();
		SectionLocalBox.Init();
		//bEnableCollision = false;
		bSectionVisible = true;
	}
};

UCLASS()
class PROCEDURALSKELETALMESH_API UProceduralSkeletalMeshComponent : public UMeshComponent //USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UProceduralSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer);

	//UActorComponent overrides
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//virtual void SendRenderDynamicData_Concurrent() override;
	//virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;

	//USceneComponent overrides
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	//UPrimitiveComponent overrides
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//virtual class UBodySetup* GetBodySetup() override;

	//UObject overrides
	virtual void PostLoad() override;

	//UMeshComponent overrides
	virtual int32 GetNumMaterials() const override;

	void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents);
	void UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents);

	//void LoadMeshData(TArray<SkeletalMeshData>& data);

private:
	/** Local space bounds of mesh */
	UPROPERTY()
	FBoxSphereBounds LocalBounds;

	void UpdateLocalBounds();

	/** Array of sections of mesh */
	UPROPERTY()
	TArray<FProcSkeletalMeshSection> ProcSkeletalMeshSections;

	friend class FProceduralSkeletalMeshSceneProxy;
};
