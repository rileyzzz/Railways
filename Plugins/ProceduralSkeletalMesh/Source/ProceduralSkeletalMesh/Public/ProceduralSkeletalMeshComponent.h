// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProceduralTypes.h"
#include "BoneIndices.h"
#include "ProceduralSkinWeightVertexBuffer.h"
//#include "ProceduralSkeletalMeshComponent.generated.h"




//USTRUCT()
//struct FProcSkeletalMeshSection
//{
//	GENERATED_BODY()
//public:
//
//	/** Vertex buffer for this section */
//	UPROPERTY()
//	TArray<FProcSkeletalMeshVertex> ProcVertexBuffer;
//
//	/** Index buffer for this section */
//	UPROPERTY()
//	TArray<uint32> ProcIndexBuffer;
//	/** Local bounding box of section */
//	UPROPERTY()
//	FBox SectionLocalBox;
//
//	UPROPERTY()
//	//TArray<FBoneIndexType> BoneMap;
//	TArray<uint16> BoneMap;
//
//	/** Should we build collision data for triangles in this section */
//	//UPROPERTY()
//	//bool bEnableCollision;
//
//	TArray<FProcSkeletalMeshSection*> Children;
//
//	/** Should we display this section */
//	UPROPERTY()
//	bool bSectionVisible;
//
//	FProcSkeletalMeshSection()
//		: SectionLocalBox(ForceInit)
//		//, bEnableCollision(false)
//		, bSectionVisible(true)
//	{}
//
//	/** Reset this section, clear all mesh info. */
//	void Reset()
//	{
//		ProcVertexBuffer.Empty();
//		ProcIndexBuffer.Empty();
//		BoneMap.Empty();
//		SectionLocalBox.Init();
//		//bEnableCollision = false;
//		bSectionVisible = true;
//	}
//};
//
//USTRUCT()
//struct FProcSkeletalMeshNode
//{
//	GENERATED_BODY()
//public:
//	TArray<FProcSkeletalMeshSection> Sections;
//	FMatrix BoneTransform;
//	TArray<FProcSkeletalMeshNode> ChildNodes;
//};
//
//UCLASS()
//class PROCEDURALSKELETALMESH_API UProceduralSkeletalMeshComponent : public UMeshComponent //USkeletalMeshComponent, USkinnedMeshComponent
//{
//	GENERATED_BODY()
//
//public:
//	UProceduralSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer);
//
//	//UActorComponent overrides
//	virtual void OnRegister() override;
//	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//	//virtual void SendRenderDynamicData_Concurrent() override;
//	//virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;
//
//	//USceneComponent overrides
//	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
//
//	//UPrimitiveComponent overrides
//	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
//	//virtual class UBodySetup* GetBodySetup() override;
//
//	//UObject overrides
//	virtual void PostLoad() override;
//
//	//UMeshComponent overrides
//	virtual int32 GetNumMaterials() const override;
//
//	void CreateMeshSection(int32 SectionIndex, FProcSkeletalMeshNode& ParentNode, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents);
//	//void UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents);
//
//	//void LoadMeshData(TArray<SkeletalMeshData>& data);
//
//	UPROPERTY()
//	FProcSkeletalMeshNode RootNode;
//
//private:
//	/** Local space bounds of mesh */
//	UPROPERTY()
//	FBoxSphereBounds LocalBounds;
//
//	void UpdateLocalBounds();
//
//	/** Array of sections of mesh */
//	//UPROPERTY()
//	//TArray<FProcSkeletalMeshSection> ProcSkeletalMeshSections;
//	
//
//	friend class FProceduralSkeletalMeshSceneProxy;
//};
