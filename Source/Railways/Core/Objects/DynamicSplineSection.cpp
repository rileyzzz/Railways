// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicSplineSection.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADynamicSplineSection::ADynamicSplineSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//RootComponent->SetMobility(EComponentMobility::Static);
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(RootComponent);
	Spline->SetMobility(EComponentMobility::Movable);
	//RefreshSpline(true);
}

#define TILE_FACTOR 400.0f

void ADynamicSplineSection::GenerateTrackTiles(int32 index, DynamicSplineSegment& Segment, UStaticMesh* Mesh, int TileCount)
{
	//clear initial segments
	for (auto tile : Segment.Tiles)
	{
		if (tile)
		{
			tile->UnregisterComponent();
			tile->DestroyComponent();
			tile = nullptr;
		}
	}
	Segment.Tiles.Empty();

	for (int Tile = 0; Tile < TileCount; Tile++)
	{
		float alpha = (float)Tile / (float)TileCount;
		float endalpha = alpha + (1.0f / (float)TileCount);

		USplineMeshComponent* NewMesh;

		NewMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), NAME_None);

		if (NewMesh)
		{
			NewMesh->RegisterComponent();
			NewMesh->SetMobility(EComponentMobility::Movable);
			NewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

			NewMesh->SetStaticMesh(Mesh);
			NewMesh->bSmoothInterpRollScale = true;

			FVector TileStart = Spline->GetLocationAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local);
			FVector TileEnd = Spline->GetLocationAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local);

			FVector TileStartTangent = Spline->GetTangentAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
			FVector TileEndTangent = Spline->GetTangentAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);

			//DrawDebugLine(GetWorld(), TileStart, TileEnd, FColor::Green, true);
			//DrawDebugLine(GetWorld(), TileStart, TileStartTangent, FColor::Blue, true);
			//DrawDebugLine(GetWorld(), TileEnd, TileEndTangent, FColor::Orange, true);

			NewMesh->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
			//UE_LOG(LogTemp, Warning, TEXT("generated tile %i"), TileID);
			Segment.Tiles.Add(NewMesh);
		}
	}
}

void ADynamicSplineSection::BuildSplineSegment(DynamicSplinePoint* Point, UStaticMesh* Mesh)
{
	int32 nextindex = (Point->index + 1) % Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Spline->GetLocalLocationAndTangentAtSplinePoint(Point->index, StartPos, StartTangent);
	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);
	
	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
	if (TileCount == 0) TileCount = 1;

	//Point->Segment = new DynamicSplineSegment();
	GenerateTrackTiles(Point->index, Point->Segment, Mesh, TileCount);
}

void ADynamicSplineSection::UpdateSplineSegment(int32 index)
{
	int32 nextindex = (index + 1) % Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Spline->GetLocalLocationAndTangentAtSplinePoint(index, StartPos, StartTangent);
	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);

	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
	if (TileCount == 0) TileCount = 1;

	DynamicSplineSegment& Segment = Points[index].Segment;
	if (Segment.Tiles.Num() == TileCount)
	{
		//refresh
		for (int Tile = 0; Tile < TileCount; Tile++)
		{
			float alpha = (float)Tile / (float)TileCount;
			float endalpha = alpha + (1.0f / (float)TileCount);

			FVector TileStart = Spline->GetLocationAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local);
			FVector TileEnd = Spline->GetLocationAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local);

			FVector TileStartTangent = Spline->GetTangentAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
			FVector TileEndTangent = Spline->GetTangentAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);

			Segment.Tiles[Tile]->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
		}
	}
	else
	{
		//regenerate block
		UE_LOG(LogTemp, Warning, TEXT("tile discrepancy old %i new %i"), Segment.Tiles.Num(), TileCount);
		GenerateTrackTiles(index, Segment, SplineMesh, TileCount);
	}

	//Segments[index]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
	//Segments[index]->UpdateMesh();
}

void ADynamicSplineSection::BuildSpline()
{
	for (auto& point : Points)
	{
		for (auto tile : point.Segment.Tiles)
		{
			if (tile)
			{
				tile->UnregisterComponent();
				tile->DestroyComponent();
				tile = nullptr;
			}
		}
	}
	for (auto& component : Dummies)
	{
		if (component)
		{
			component->UnregisterComponent();
			component->DestroyComponent();
			component = nullptr;
		}
	}
	for (auto& component : Decals)
	{
		if (component)
		{
			component->UnregisterComponent();
			component->DestroyComponent();
			component = nullptr;
		}
	}
	Points.Empty();
	Decals.Empty();
	Dummies.Empty();

	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		DynamicSplinePoint NewPoint(this, i);

		if (i != Spline->GetNumberOfSplinePoints() - 1)
			BuildSplineSegment(&NewPoint, SplineMesh);

		UDecalComponent* Decal = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass(), NAME_None);
		if (Decal)
		{
			Decal->SetDecalMaterial(DecalMaterial);
			Decal->RegisterComponent();
			Decal->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			Decal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
			Decal->DecalSize = FVector(128.0f, 128.0f, 128.0f);
			FVector PointLocation;
			FVector PointTangent;
			Spline->GetLocalLocationAndTangentAtSplinePoint(i, PointLocation, PointTangent);
			Decal->SetRelativeLocation(PointLocation);
			Decals.Add(Decal);

			USphereComponent* Dummy = NewObject<USphereComponent>(this, USphereComponent::StaticClass(), NAME_None);
			Dummy->RegisterComponent();
			Dummy->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
			Dummy->SetCollisionProfileName(TEXT("InitialHitOnly"));
			Dummy->AttachToComponent(Decal, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			Dummy->SetSphereRadius(128.0f);
			Dummies.Add(Dummy);
		}

		Points.Add(NewPoint);
	}
}


void ADynamicSplineSection::EnableCollision()
{
	for (auto& point : Points)
	{
		for (auto& tile : point.Segment.Tiles)
			tile->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	}	
}

void ADynamicSplineSection::DisableCollision()
{
	for (auto& point : Points)
	{
		for (auto& tile : point.Segment.Tiles)
			tile->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}
}


void ADynamicSplineSection::RefreshSpline()
{
	for (int i = 0; i < Points.Num(); i++)
	{
		if (i != Points.Num() - 1)
			UpdateSplineSegment(i);

		FVector PointLocation;
		FVector PointTangent;
		Spline->GetLocalLocationAndTangentAtSplinePoint(i, PointLocation, PointTangent);
		Decals[i]->SetRelativeLocation(PointLocation);
	}
}
//
//void ADynamicSplineSection::BranchJunction(DynamicSplinePoint* A, DynamicSplinePoint* B)
//{
//	A->Junction = B;
//	B->Junction = A;
//}


// Called when the game starts or when spawned
void ADynamicSplineSection::BeginPlay()
{
	Super::BeginPlay();
	//RefreshSpline(false);
	BuildSpline();
}

// Called every frame
void ADynamicSplineSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void ADynamicSplineSection::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	//RefreshSpline(false);
}
#endif