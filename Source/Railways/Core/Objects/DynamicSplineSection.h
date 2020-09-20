// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeActorAsset.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "DynamicSplineSection.generated.h"

//struct SplinePointMetadata
//{
//	float SplineRoll = 0.0f;
//};

struct DynamicSplineSegment;
struct DynamicSplinePoint;

UCLASS()
class RAILWAYS_API ADynamicSplineSection : public ARuntimeActorAsset
{
	GENERATED_BODY()
	
private:
	void GenerateTrackTiles(int32 index, DynamicSplineSegment& Segment, UStaticMesh* Mesh, int TileCount);
	
	void BuildSplineSegment(DynamicSplinePoint* Point, UStaticMesh* Mesh);
	void UpdateSplineSegment(int32 index);
public:
	USplineComponent* Spline;
	//TMap<int32, SplinePointMetadata> PointMetadata;
	TArray<DynamicSplinePoint> Points;
	TArray<UDecalComponent*> Decals;
	TArray<USphereComponent*> Dummies;

	UPROPERTY(EditAnywhere)
	UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DecalMaterial;

	void BuildSpline();
	void EnableCollision();
	void DisableCollision();
	void RefreshSpline();

	//void BranchJunction(DynamicSplinePoint* A, DynamicSplinePoint* B);

	// Sets default values for this actor's properties
	ADynamicSplineSection();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

struct DynamicSplineSegment
{
	TArray<USplineMeshComponent*> Tiles;
};

struct DynamicSplinePoint
{
	int32 index;
	ADynamicSplineSection* ParentSection;
	ADynamicSplineSection* JunctionSection = nullptr;
	DynamicSplineSegment Segment;
	DynamicSplinePoint(ADynamicSplineSection* in_parent, int32 in_index) : ParentSection(in_parent), index(in_index) { }
	FVector GetWorldLocation()
	{
		return ParentSection->Spline->GetWorldLocationAtSplinePoint(index);
	}
	DynamicSplinePoint* GetJunction()
	{
		if (JunctionSection)
		{
			int Point = FMath::RoundToInt(JunctionSection->Spline->FindInputKeyClosestToWorldLocation(GetWorldLocation()));
			return &JunctionSection->Points[Point];
		}
		else
			return nullptr;
	}
	~DynamicSplinePoint()
	{
		//clear associated junction
		auto Junction = GetJunction();
		if (Junction) Junction->JunctionSection = nullptr;
	}
public:
	void CreateJunction(DynamicSplinePoint* Other)
	{
		JunctionSection = Other->ParentSection;
		Other->JunctionSection = ParentSection;
	}
	
	void SetWorldLocation(FVector& InLocation)
	{
		ParentSection->Spline->SetWorldLocationAtSplinePoint(index, InLocation);
		ParentSection->RefreshSpline();
		auto Junction = GetJunction();
		if (Junction)
		{
			Junction->ParentSection->Spline->SetWorldLocationAtSplinePoint(Junction->index, InLocation);
			Junction->ParentSection->RefreshSpline();
		}
	}
	void SetTangent(FVector& InTangent, ESplineCoordinateSpace::Type CoordSpace)
	{
		ParentSection->Spline->SetTangentAtSplinePoint(index, InTangent, CoordSpace);
		ParentSection->RefreshSpline();
	}
	FVector GetTangent(ESplineCoordinateSpace::Type CoordSpace)
	{
		return ParentSection->Spline->GetTangentAtSplinePoint(index, CoordSpace);
	}
};