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

UCLASS()
class RAILWAYS_API ADynamicSplineSection : public ARuntimeActorAsset
{
	GENERATED_BODY()
	
private:
	//void GenerateTrackTiles(int32 index, DynamicSplineSegment& Segment, UStaticMesh* Mesh, int TileCount);
	//
	//void BuildSplineSegment(DynamicSplinePoint* Point, UStaticMesh* Mesh);
	//void UpdateSplineSegment(int32 index);
public:
	USplineComponent* Spline;
	//TMap<int32, SplinePointMetadata> PointMetadata;
	//TArray<DynamicSplinePoint> Points;
	TArray<UDecalComponent*> Decals;
	TArray<USphereComponent*> Dummies;

	UPROPERTY(EditAnywhere)
	UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DecalMaterial;

	UDynamicSplinePoint* RootPoint;
	void RefreshSection();
	//void BuildSpline();
	//void EnableCollision();
	//void DisableCollision();
	//void RefreshSpline();

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

	//USceneComponent* GetRootComponent();
};

struct DynamicSplineSegment
{
	TArray<USplineMeshComponent*> Tiles;
};

UCLASS()
class RAILWAYS_API UDynamicSplinePoint : public USceneComponent
{
	GENERATED_BODY()

public:
	int32 index;
	TArray<UDynamicSplinePoint*> Paths;
	UStaticMesh* Mesh;

	FVector Location;
	ADynamicSplineSection* ParentSection;

private:
	//used only on the root, for initial placement purposes
	USplineComponent* RootStartSpline;

	TArray<USplineComponent*> BranchSplines;
	DynamicSplineSegment Segment;
	UDecalComponent* Decal;
	USphereComponent* Dummy;

	void ClearBranches();

	void GenerateTiles(USplineComponent* Spline, int TileCount);
	void BuildSegment(int32 SegmentIndex, USplineComponent* Spline);
	void UpdateSegment(int32 SegmentIndex, USplineComponent* Spline);
	void RecursiveBuild(USplineComponent* Spline, TArray<UDynamicSplinePoint*>& Coverage);
	void RecursiveRefresh(USplineComponent* Spline, TArray<UDynamicSplinePoint*>& Coverage);
public:
	UDynamicSplinePoint();

	FVector GetWorldLocation()
	{
		return Location;
	}
	void SetWorldLocation(FVector& InLocation)
	{
		Location = InLocation;
		//RootRefresh();
		//ParentSection->RootPoint->RootRefresh();
	}

	void SetTangent(FVector& InTangent, ESplineCoordinateSpace::Type CoordSpace)
	{
		//ParentSection->Spline->SetTangentAtSplinePoint(index, InTangent, CoordSpace);
		//ParentSection->RefreshSpline();
	}
	FVector GetTangent(ESplineCoordinateSpace::Type CoordSpace)
	{
		return FVector();
		//return ParentSection->Spline->GetTangentAtSplinePoint(index, CoordSpace);
	}

	void RootBuild();
	
	void RootRefresh();
};