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
	//Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	//Spline->SetupAttachment(RootComponent);
	//Spline->SetMobility(EComponentMobility::Movable);
	//RootPoint = CreateDefaultSubobject<DynamicSplinePoint>(TEXT("PointComponent"));
	//RootPoint->SetupAttachment(RootComponent);
	//DynamicSplinePoint* SecondPoint = CreateDefaultSubobject<DynamicSplinePoint>(TEXT("PointComponent2"));
	//SecondPoint->SetupAttachment(RootComponent);

	//FVector SecondLocation(100.0f, 100.0f, 0.0f);
	//SecondPoint->SetWorldLocation(SecondLocation);
	//RootPoint->Paths.Add(SecondPoint);
	//RefreshSpline(true);
}

void ADynamicSplineSection::RefreshSection()
{
	RootPoint->RootRefresh();
}

#define TILE_FACTOR 400.0f

//void ADynamicSplineSection::GenerateTrackTiles(int32 index, DynamicSplineSegment& Segment, UStaticMesh* Mesh, int TileCount)
//{
//	//clear initial segments
//	for (auto tile : Segment.Tiles)
//	{
//		if (tile)
//		{
//			tile->UnregisterComponent();
//			tile->DestroyComponent();
//			tile = nullptr;
//		}
//	}
//	Segment.Tiles.Empty();
//
//	for (int Tile = 0; Tile < TileCount; Tile++)
//	{
//		float alpha = (float)Tile / (float)TileCount;
//		float endalpha = alpha + (1.0f / (float)TileCount);
//
//		USplineMeshComponent* NewMesh;
//
//		NewMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), NAME_None);
//
//		if (NewMesh)
//		{
//			NewMesh->RegisterComponent();
//			NewMesh->SetMobility(EComponentMobility::Movable);
//			NewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
//
//			NewMesh->SetStaticMesh(Mesh);
//			NewMesh->bSmoothInterpRollScale = true;
//
//			FVector TileStart = Spline->GetLocationAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local);
//			FVector TileEnd = Spline->GetLocationAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local);
//
//			FVector TileStartTangent = Spline->GetTangentAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
//			FVector TileEndTangent = Spline->GetTangentAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
//
//			//DrawDebugLine(GetWorld(), TileStart, TileEnd, FColor::Green, true);
//			//DrawDebugLine(GetWorld(), TileStart, TileStartTangent, FColor::Blue, true);
//			//DrawDebugLine(GetWorld(), TileEnd, TileEndTangent, FColor::Orange, true);
//
//			NewMesh->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
//			//UE_LOG(LogTemp, Warning, TEXT("generated tile %i"), TileID);
//			Segment.Tiles.Add(NewMesh);
//		}
//	}
//}
//
//void ADynamicSplineSection::BuildSplineSegment(DynamicSplinePoint* Point, UStaticMesh* Mesh)
//{
//	int32 nextindex = (Point->index + 1) % Spline->GetNumberOfSplinePoints();
//	FVector StartPos, StartTangent;
//	FVector EndPos, EndTangent;
//	Spline->GetLocalLocationAndTangentAtSplinePoint(Point->index, StartPos, StartTangent);
//	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);
//	
//	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
//	if (TileCount == 0) TileCount = 1;
//
//	//Point->Segment = new DynamicSplineSegment();
//	GenerateTrackTiles(Point->index, Point->Segment, Mesh, TileCount);
//}
//
//void ADynamicSplineSection::UpdateSplineSegment(int32 index)
//{
//	int32 nextindex = (index + 1) % Spline->GetNumberOfSplinePoints();
//	FVector StartPos, StartTangent;
//	FVector EndPos, EndTangent;
//	Spline->GetLocalLocationAndTangentAtSplinePoint(index, StartPos, StartTangent);
//	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);
//
//	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
//	if (TileCount == 0) TileCount = 1;
//
//	DynamicSplineSegment& Segment = Points[index].Segment;
//	if (Segment.Tiles.Num() == TileCount)
//	{
//		//refresh
//		for (int Tile = 0; Tile < TileCount; Tile++)
//		{
//			float alpha = (float)Tile / (float)TileCount;
//			float endalpha = alpha + (1.0f / (float)TileCount);
//
//			FVector TileStart = Spline->GetLocationAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local);
//			FVector TileEnd = Spline->GetLocationAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local);
//
//			FVector TileStartTangent = Spline->GetTangentAtSplineInputKey((float)index + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
//			FVector TileEndTangent = Spline->GetTangentAtSplineInputKey((float)index + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
//
//			Segment.Tiles[Tile]->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
//		}
//	}
//	else
//	{
//		//regenerate block
//		UE_LOG(LogTemp, Warning, TEXT("tile discrepancy old %i new %i"), Segment.Tiles.Num(), TileCount);
//		GenerateTrackTiles(index, Segment, SplineMesh, TileCount);
//	}
//
//	//Segments[index]->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, false);
//	//Segments[index]->UpdateMesh();
//}
//
//void ADynamicSplineSection::BuildSpline()
//{
//	TMap<FVector, ADynamicSplineSection*> PersistJunctions;
//	for (auto& point : Points)
//	{
//		for (auto tile : point.Segment.Tiles)
//		{
//			if (tile)
//			{
//				tile->UnregisterComponent();
//				tile->DestroyComponent();
//				tile = nullptr;
//			}
//		}
//		if (point.JunctionSection)
//		{
//			PersistJunctions.Add(point.GetWorldLocation(), point.JunctionSection);
//		}
//	}
//	for (auto& component : Dummies)
//	{
//		if (component)
//		{
//			component->UnregisterComponent();
//			component->DestroyComponent();
//			component = nullptr;
//		}
//	}
//	for (auto& component : Decals)
//	{
//		if (component)
//		{
//			component->UnregisterComponent();
//			component->DestroyComponent();
//			component = nullptr;
//		}
//	}
//	Points.Empty();
//	Decals.Empty();
//	Dummies.Empty();
//
//	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
//	{
//		DynamicSplinePoint NewPoint(this, i);
//
//		if (i != Spline->GetNumberOfSplinePoints() - 1)
//			BuildSplineSegment(&NewPoint, SplineMesh);
//
//		UDecalComponent* Decal = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass(), NAME_None);
//		if (Decal)
//		{
//			Decal->SetDecalMaterial(DecalMaterial);
//			Decal->RegisterComponent();
//			Decal->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
//			Decal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
//			Decal->DecalSize = FVector(128.0f, 128.0f, 128.0f);
//			FVector PointLocation;
//			FVector PointTangent;
//			Spline->GetLocalLocationAndTangentAtSplinePoint(i, PointLocation, PointTangent);
//			Decal->SetRelativeLocation(PointLocation);
//			Decals.Add(Decal);
//
//			USphereComponent* Dummy = NewObject<USphereComponent>(this, USphereComponent::StaticClass(), NAME_None);
//			Dummy->RegisterComponent();
//			Dummy->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
//			Dummy->SetCollisionProfileName(TEXT("InitialHitOnly"));
//			Dummy->AttachToComponent(Decal, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
//			Dummy->SetSphereRadius(128.0f);
//			Dummies.Add(Dummy);
//		}
//
//		FVector Location = NewPoint.GetWorldLocation();
//		//if (PersistJunctions.Contains(Location)) NewPoint.JunctionSection = PersistJunctions[Location];
//		for (auto& junction : PersistJunctions)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Junction dist %f"), FVector::Distance(Location, junction.Key));
//			if (FVector::Distance(Location, junction.Key) < 0.5f)
//			{
//				NewPoint.JunctionSection = junction.Value;
//				NewPoint.GetJunction()->JunctionSection = this;
//				break;
//			}
//		}
//
//		Points.Add(NewPoint);
//	}
//}
//
//
//void ADynamicSplineSection::EnableCollision()
//{
//	for (auto& point : Points)
//	{
//		for (auto& tile : point.Segment.Tiles)
//			tile->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
//	}	
//}
//
//void ADynamicSplineSection::DisableCollision()
//{
//	for (auto& point : Points)
//	{
//		for (auto& tile : point.Segment.Tiles)
//			tile->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
//	}
//}
//
//
//void ADynamicSplineSection::RefreshSpline()
//{
//	for (int i = 0; i < Points.Num(); i++)
//	{
//		if (i != Points.Num() - 1)
//			UpdateSplineSegment(i);
//
//		FVector PointLocation;
//		FVector PointTangent;
//		Spline->GetLocalLocationAndTangentAtSplinePoint(i, PointLocation, PointTangent);
//		Decals[i]->SetRelativeLocation(PointLocation);
//	}
//}
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

	RootPoint = NewObject<UDynamicSplinePoint>(this, UDynamicSplinePoint::StaticClass(), NAME_None);
	RootPoint->ParentSection = this;
	RootPoint->Mesh = SplineMesh;
	RootPoint->RegisterComponent();
	RootPoint->SetMobility(EComponentMobility::Movable);
	RootPoint->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));


	UDynamicSplinePoint* SecondPoint = NewObject<UDynamicSplinePoint>(this, UDynamicSplinePoint::StaticClass(), NAME_None);
	SecondPoint->ParentSection = this;
	SecondPoint->Mesh = SplineMesh;
	SecondPoint->RegisterComponent();
	SecondPoint->SetMobility(EComponentMobility::Movable);
	SecondPoint->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	UDynamicSplinePoint* ThirdPoint = NewObject<UDynamicSplinePoint>(this, UDynamicSplinePoint::StaticClass(), NAME_None);
	ThirdPoint->ParentSection = this;
	ThirdPoint->Mesh = SplineMesh;
	ThirdPoint->RegisterComponent();
	ThirdPoint->SetMobility(EComponentMobility::Movable);
	ThirdPoint->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	
	UDynamicSplinePoint* FourthPoint = NewObject<UDynamicSplinePoint>(this, UDynamicSplinePoint::StaticClass(), NAME_None);
	FourthPoint->ParentSection = this;
	FourthPoint->Mesh = SplineMesh;
	FourthPoint->RegisterComponent();
	FourthPoint->SetMobility(EComponentMobility::Movable);
	FourthPoint->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	FVector FirstLocation(0.0f, 0.0f, 50.0f);
	FVector SecondLocation(100.0f, 100.0f, 50.0f);
	FVector ThirdLocation(300.0f, 200.0f, 50.0f);
	FVector FourthLocation(500.0f, 300.0f, 50.0f);
	RootPoint->SetWorldLocation(FirstLocation);
	SecondPoint->SetWorldLocation(SecondLocation);
	ThirdPoint->SetWorldLocation(ThirdLocation);
	FourthPoint->SetWorldLocation(FourthLocation);
	RootPoint->Paths.Add(SecondPoint);
	SecondPoint->Paths.Add(ThirdPoint);
	ThirdPoint->Paths.Add(FourthPoint);

	FourthPoint->Paths.Add(ThirdPoint);
	ThirdPoint->Paths.Add(SecondPoint);
	SecondPoint->Paths.Add(RootPoint);

	RootPoint->RootBuild();
	//RefreshSpline(false);
	//BuildSpline();
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


UDynamicSplinePoint::UDynamicSplinePoint()
{

}

void UDynamicSplinePoint::ClearBranches()
{
	for (auto& Spline : BranchSplines)
	{
		Spline->UnregisterComponent();
		Spline->DestroyComponent();
		Spline = nullptr;
	}
	BranchSplines.Empty();
}

void UDynamicSplinePoint::RecursiveBuild(USplineComponent* Spline, TArray<UDynamicSplinePoint*>& Coverage, UDynamicSplinePoint* Last)
{
	int32 index = Spline->GetNumberOfSplinePoints();
	//add current point to spline
	Spline->AddSplinePoint(GetComponentTransform().GetLocation(), ESplineCoordinateSpace::World);
	if (Last) ParentSection->Segments.Emplace(Last, this, index - 1, Spline);

	ClearBranches();
	Coverage.Add(this);
	TArray<UDynamicSplinePoint*> NewBranches;
	for (auto& NewBranch : Paths)
	{
		if (!Coverage.Contains(NewBranch))
		{
			NewBranches.Add(NewBranch);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Branch already discovered!"));
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Spline has %i paths and %i new branches"), Paths.Num(), NewBranches.Num());

	
	
	if (NewBranches.Num() == 1)
	{
		NewBranches[0]->RecursiveBuild(Spline, Coverage, this);
	}
	else
	{
		TArray<FVector> BranchTangents;
		for (auto& NewBranch : NewBranches)
		{
			//junction
			USplineComponent* NewSpline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), NAME_None);

			if (NewSpline)
			{
				NewSpline->RegisterComponent();
				NewSpline->SetMobility(EComponentMobility::Movable);
				NewSpline->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
				NewSpline->ClearSplinePoints();
				NewSpline->AddSplinePoint(GetComponentToWorld().GetLocation(), ESplineCoordinateSpace::World);
				FVector PointTangent = Spline->GetTangentAtSplinePoint(index, ESplineCoordinateSpace::World);
				NewSpline->SetTangentAtSplinePoint(0, PointTangent, ESplineCoordinateSpace::World);

				NewBranch->RecursiveBuild(NewSpline, Coverage, this);

				//get initial tangent after build
				BranchTangents.Add(NewSpline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World));

				BranchSplines.Add(NewSpline);
			}
		}

		if (BranchTangents.Num())
		{
			//FVector TangentAverage;
			//for (auto& BranchTangent : BranchTangents)
			//	TangentAverage += BranchTangent;
			//TangentAverage /= BranchTangents.Num();
			Spline->SetTangentAtSplinePoint(index, BranchTangents[0], ESplineCoordinateSpace::World);
		}

		//Generate mesh if a dead end or a split
		UE_LOG(LogTemp, Warning, TEXT("attempting to build %i segments"), Spline->GetNumberOfSplinePoints() - 1);
		/*for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++)
			BuildSegment(i, Spline);*/
		/*for (int i = 0; i < ParentSection->Segments.Num(); i++)
			BuildSegment(i, Spline);*/
		for (auto& segment : ParentSection->Segments)
			BuildSegment(&segment);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Spline has %i branches"), BranchSplines.Num());
	////Generate mesh
	//for (auto& Spline : BranchSplines)
	//{
	//	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	//	{
	//		if (i != Spline->GetNumberOfSplinePoints() - 1)
	//			BuildSegment(Spline);
	//	}
	//}

	Decal = NewObject<UDecalComponent>(this, UDecalComponent::StaticClass(), NAME_None);
	if (Decal)
	{
		Decal->SetDecalMaterial(ParentSection->DecalMaterial);
		Decal->RegisterComponent();
		Decal->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Decal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
		Decal->DecalSize = FVector(128.0f, 128.0f, 128.0f);
		//Decal->SetWorldLocation(Location);
	}

	Dummy = NewObject<USphereComponent>(this, USphereComponent::StaticClass(), NAME_None);
	if (Dummy)
	{
		Dummy->RegisterComponent();
		Dummy->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		Dummy->SetCollisionProfileName(TEXT("InitialHitOnly"));
		Dummy->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Dummy->SetSphereRadius(128.0f);
	}
}

//void UDynamicSplinePoint::RecursiveRefresh(USplineComponent* Spline, TArray<UDynamicSplinePoint*>& Coverage)
//{
//	Spline->SetWorldLocationAtSplinePoint(++index, GetComponentTransform().GetLocation());
//	
//	Coverage.Add(this);
//	TArray<UDynamicSplinePoint*> NewBranches;
//	for (auto NewBranch : Paths)
//	{
//		if (!Coverage.Contains(NewBranch))
//			NewBranches.Add(NewBranch);
//	}
//
//	if (NewBranches.Num() == 1)
//	{
//		//continue down the spline
//		NewBranches[0]->RecursiveRefresh(Spline, index, Coverage);
//	}
//	else
//	{
//		for (int i = 0; i < NewBranches.Num(); i++)
//		{
//			auto& Path = NewBranches[i];
//			auto& Branch = BranchSplines[i];
//			int32 NewStartIndex = 0;
//			Path->RecursiveRefresh(Branch, NewStartIndex, Coverage);
//		}
//
//		//clear initial segments, need to do this here so we dont destroy previous tiles in the segment updater
//		//for (auto tile : Segment.Tiles)
//		//{
//		//	if (tile)
//		//	{
//		//		tile->UnregisterComponent();
//		//		tile->DestroyComponent();
//		//		tile = nullptr;
//		//	}
//		//}
//		//Segment.Tiles.Empty();
//
//		//dead end or split, update mesh
//		/*for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++)
//			UpdateSegment(i, Spline);*/
//		for(auto& segment : ParentSection->Segments)
//			UpdateSegment(&segment);
//	}
//}

void UDynamicSplinePoint::RefreshSegment(DynamicSplineSegment* Segment)
{
	FVector StartPos = Segment->StartPoint->GetComponentTransform().GetLocation();
	FVector EndPos = Segment->EndPoint->GetComponentTransform().GetLocation();
	Segment->Spline->SetWorldLocationAtSplinePoint(Segment->StartIndex, StartPos);
	Segment->Spline->SetWorldLocationAtSplinePoint(Segment->StartIndex + 1, EndPos);

	if (Segment->StartPoint->Paths.Num() > 2)
	{
		FVector StartTangent = Segment->Spline->GetTangentAtSplinePoint(Segment->StartIndex, ESplineCoordinateSpace::World);
		float StartDirection = FMath::Sign(FVector::DotProduct(StartTangent, EndPos - StartPos));
		if (StartDirection == 0) StartDirection = 1;
		Segment->Spline->SetTangentAtSplinePoint(Segment->StartIndex, StartTangent * StartDirection, ESplineCoordinateSpace::World);
	}
	
	UpdateSegment(Segment);
}

//Segment index is index into the spline!
void UDynamicSplinePoint::BuildSegment(DynamicSplineSegment* Segment)
{
	int32 SegmentIndex = Segment->StartIndex;
	UE_LOG(LogTemp, Warning, TEXT("BUILDING A SEGMENT %i"), SegmentIndex);
	int32 nextindex = (SegmentIndex + 1) % Segment->Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Segment->Spline->GetLocalLocationAndTangentAtSplinePoint(SegmentIndex, StartPos, StartTangent);
	Segment->Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);

	UE_LOG(LogTemp, Warning, TEXT("start %f %f %f end %f %f %f"), StartPos.X, StartPos.Y, StartPos.Z, EndPos.X, EndPos.Y, EndPos.Z);

	//DrawDebugLine(GetWorld(), StartPos, StartPos + FVector(0.0f, 0.0f, 500.0f), FColor::Blue, true);
	//DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, true);

	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
	if (TileCount == 0) TileCount = 1;

	GenerateTiles(Segment, TileCount);
}

void UDynamicSplinePoint::UpdateSegment(DynamicSplineSegment* Segment)
{
	int32 SegmentIndex = Segment->StartIndex;
	int32 nextindex = (SegmentIndex + 1) % Segment->Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Segment->Spline->GetLocalLocationAndTangentAtSplinePoint(SegmentIndex, StartPos, StartTangent);
	Segment->Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);

	int TileCount = FVector::Distance(StartPos, EndPos) / TILE_FACTOR;
	if (TileCount == 0) TileCount = 1;

	if (Segment->Tiles.Num() == TileCount)
	{
		//refresh
		for (int Tile = 0; Tile < TileCount; Tile++)
		{
			float alpha = (float)Tile / (float)TileCount;
			float endalpha = alpha + (1.0f / (float)TileCount);

			FVector TileStart = Segment->Spline->GetLocationAtSplineInputKey((float)SegmentIndex + alpha, ESplineCoordinateSpace::Local);
			FVector TileEnd = Segment->Spline->GetLocationAtSplineInputKey((float)SegmentIndex + endalpha, ESplineCoordinateSpace::Local);

			FVector TileStartTangent = Segment->Spline->GetTangentAtSplineInputKey((float)SegmentIndex + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
			FVector TileEndTangent = Segment->Spline->GetTangentAtSplineInputKey((float)SegmentIndex + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);

			Segment->Tiles[Tile]->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
		}
	}
	else
	{
		//regenerate block
		UE_LOG(LogTemp, Warning, TEXT("tile discrepancy old %i new %i"), Segment->Tiles.Num(), TileCount);
		GenerateTiles(Segment, TileCount);
	}
}

void UDynamicSplinePoint::GenerateTiles(DynamicSplineSegment* Segment, int TileCount)
{
	for (auto tile : Segment->Tiles)
	{
		if (tile)
		{
			tile->UnregisterComponent();
			tile->DestroyComponent();
			tile = nullptr;
		}
	}
	Segment->Tiles.Empty();

	int32 SegmentIndex = Segment->StartIndex;
	for (int Tile = 0; Tile < TileCount; Tile++)
	{
		float alpha = (float)Tile / (float)TileCount;
		float endalpha = alpha + (1.0f / (float)TileCount);

		USplineMeshComponent* NewMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), NAME_None);

		if (NewMesh)
		{
			NewMesh->RegisterComponent();
			NewMesh->SetMobility(EComponentMobility::Movable);
			//NewMesh->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
			NewMesh->AttachToComponent(Segment->Spline, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

			NewMesh->SetStaticMesh(Mesh);
			NewMesh->bSmoothInterpRollScale = true;

			FVector TileStart = Segment->Spline->GetLocationAtSplineInputKey((float)SegmentIndex + alpha, ESplineCoordinateSpace::Local);
			FVector TileEnd = Segment->Spline->GetLocationAtSplineInputKey((float)SegmentIndex + endalpha, ESplineCoordinateSpace::Local);

			FVector TileStartTangent = Segment->Spline->GetTangentAtSplineInputKey((float)SegmentIndex + alpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);
			FVector TileEndTangent = Segment->Spline->GetTangentAtSplineInputKey((float)SegmentIndex + endalpha, ESplineCoordinateSpace::Local) * (1.0f / (float)TileCount);

			//UE_LOG(LogTemp, Warning, TEXT("creating tile from %f %f %f to %f %f %f"), TileStart.X, TileStart.Y, TileStart.Z, TileEnd.X, TileEnd.Y, TileEnd.Z);
			//UE_LOG(LogTemp, Warning, TEXT("tangent from %f %f %f to %f %f %f"), TileStartTangent.X, TileStartTangent.Y, TileStartTangent.Z, TileEndTangent.X, TileEndTangent.Y, TileEndTangent.Z);

			//DrawDebugLine(GetWorld(), TileStart, TileEnd, FColor::Green, true);


			NewMesh->SetStartAndEnd(TileStart, TileStartTangent, TileEnd, TileEndTangent);
			UE_LOG(LogTemp, Warning, TEXT("generated tile %i"), Tile);
			Segment->Tiles.Add(NewMesh);
		}
	}
}

void UDynamicSplinePoint::RootBuild()
{
	for (auto& Segment : ParentSection->Segments)
	{
		for (auto tile : Segment.Tiles)
		{
			if (tile)
			{
				tile->UnregisterComponent();
				tile->DestroyComponent();
				tile = nullptr;
			}
		}
	}
	ParentSection->Segments.Empty();

	RootStartSpline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), NAME_None);

	if (RootStartSpline)
	{
		RootStartSpline->RegisterComponent();
		RootStartSpline->SetMobility(EComponentMobility::Movable);
		RootStartSpline->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		RootStartSpline->ClearSplinePoints();
		//BranchSplines.Add(RootStartSpline);
		TArray<UDynamicSplinePoint*> Coverage;
		RecursiveBuild(RootStartSpline, Coverage);
	}
}

void UDynamicSplinePoint::RootRefresh()
{
	TArray<UDynamicSplinePoint*> Coverage;
	//RecursiveRefresh(RootStartSpline, StartIndex, Coverage);
	for (auto& Segment : ParentSection->Segments)
	{
		RefreshSegment(&Segment);
		//Segment.Spline->SetWorldLocationAtSplinePoint(Segment.StartIndex, GetComponentTransform().GetLocation());
	}
	/*for (auto& Path : Paths)
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
		{
			if (i != Spline->GetNumberOfSplinePoints() - 1)
				UpdateSegment(i, Spline);
		}
	}*/
	
	//Decal->SetRelativeLocation(Location); this is attached now
}
