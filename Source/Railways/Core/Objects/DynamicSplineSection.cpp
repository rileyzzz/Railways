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

void ADynamicSplineSection::GenerateTrackTiles(int32 index, TArray<USplineMeshComponent*>& Segment, UStaticMesh* Mesh, int TileCount, bool Constructor)
{
	//clear initial segments
	for (auto& tile : Segment)
	{
		if (tile)
		{
			tile->UnregisterComponent();
			tile->DestroyComponent();
			tile = nullptr;
		}
	}
	Segment.Empty();

	for (int Tile = 0; Tile < TileCount; Tile++)
	{
		static int TileID = 0;
		TileID++;

		float alpha = (float)Tile / (float)TileCount;
		float endalpha = alpha + (1.0f / (float)TileCount);

		FString ComponentName = FString::Printf(TEXT("SplineMesh_%i"), TileID);
		USplineMeshComponent* NewMesh;
		if (Constructor)
			NewMesh = CreateDefaultSubobject<USplineMeshComponent>(*ComponentName);
		else
			NewMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), NAME_None);

		if (NewMesh)
		{
			NewMesh->SetMobility(EComponentMobility::Movable);
			if (Constructor)
				NewMesh->SetupAttachment(RootComponent);
			else
			{
				NewMesh->RegisterComponent();
				NewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			}

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
			Segment.Add(NewMesh);
		}
	}
}

void ADynamicSplineSection::BuildSplineSegment(int32 index, UStaticMesh* Mesh, bool Constructor)
{
	int32 nextindex = (index + 1) % Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Spline->GetLocalLocationAndTangentAtSplinePoint(index, StartPos, StartTangent);
	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);
	
	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
	if (TileCount == 0) TileCount = 1;

	TArray<USplineMeshComponent*> Segment;
	GenerateTrackTiles(index, Segment, Mesh, TileCount, Constructor);
	Segments.Add(Segment);
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

	auto& Segment = Segments[index];
	if (Segment.Num() == TileCount)
	{
		//refresh
		for(auto& tile : Segment)
			tile->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	}
	else
	{
		//regenerate block
		UE_LOG(LogTemp, Warning, TEXT("tile discrepancy old %i new %i"), Segment.Num(), TileCount);
		GenerateTrackTiles(index, Segment, SplineMesh, TileCount, false);
	}

	//Segments[index]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
	//Segments[index]->UpdateMesh();
}

void ADynamicSplineSection::BuildSpline(bool Constructor)
{
	for (auto& component : Segments)
	{
		for (auto& tile : component)
		{
			if (tile)
			{
				tile->UnregisterComponent();
				tile->DestroyComponent();
				tile = nullptr;
			}
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
	Segments.Empty();
	Decals.Empty();

	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		if (i != Spline->GetNumberOfSplinePoints() - 1)
			BuildSplineSegment(i, SplineMesh, Constructor);

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
			
			Dummy->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
			Dummy->SetCollisionProfileName(TEXT("InitialHitOnly"));
			//Dummy->SetCollisionProfileName(TEXT("BlockAll"));
			//Dummy->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			//Decal->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			Dummy->AttachToComponent(Decal, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			Dummy->SetSphereRadius(128.0f);
			//Dummy->SetGenerateOverlapEvents(true);
			//Dummy->SetRelativeLocation(PointLocation);
			Dummies.Add(Dummy);
		}
	}
	//EnableCollision();

	CachedSplineCount = Spline->GetNumberOfSplinePoints();
}


void ADynamicSplineSection::EnableCollision()
{
	for (auto& segment : Segments)
		for(auto& tile : segment)
			tile->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
}

void ADynamicSplineSection::DisableCollision()
{
	for (auto& segment : Segments)
		for (auto& tile : segment)
			tile->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}


void ADynamicSplineSection::RefreshSpline()
{
	for (int i = 0; i < CachedSplineCount; i++)
	{
		if (i != CachedSplineCount - 1)
			UpdateSplineSegment(i);

		FVector PointLocation;
		FVector PointTangent;
		Spline->GetLocalLocationAndTangentAtSplinePoint(i, PointLocation, PointTangent);
		Decals[i]->SetRelativeLocation(PointLocation);
	}
}


// Called when the game starts or when spawned
void ADynamicSplineSection::BeginPlay()
{
	Super::BeginPlay();
	//RefreshSpline(false);
	BuildSpline(false);
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