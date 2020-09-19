// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicSplineSection.h"

// Sets default values
ADynamicSplineSection::ADynamicSplineSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//RootComponent->SetMobility(EComponentMobility::Static);
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(RootComponent);

	//RefreshSpline(true);
}

void ADynamicSplineSection::BuildSplineSegment(int32 index, UStaticMesh* Mesh, bool Constructor)
{
	UE_LOG(LogTemp, Log, TEXT("constructing spline segment %i"), index);
	int32 nextindex = (index + 1) % Spline->GetNumberOfSplinePoints();
	FVector StartPos, StartTangent;
	FVector EndPos, EndTangent;
	Spline->GetLocalLocationAndTangentAtSplinePoint(index, StartPos, StartTangent);
	Spline->GetLocalLocationAndTangentAtSplinePoint(nextindex, EndPos, EndTangent);

	FString ComponentName = FString::Printf(TEXT("SplineMesh_%i"), index);
	USplineMeshComponent* NewMesh;
	if(Constructor)
		NewMesh = CreateDefaultSubobject<USplineMeshComponent>(*ComponentName);
	else
		NewMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), NAME_None);

	if (NewMesh)
	{
		NewMesh->SetMobility(EComponentMobility::Movable);
		if(Constructor)
			NewMesh->SetupAttachment(RootComponent);
		else
		{
			NewMesh->RegisterComponent();
			NewMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		}

		NewMesh->SetStaticMesh(Mesh);
		NewMesh->bSmoothInterpRollScale = true;
		//NewMesh->SetForwardAxis(ESplineMeshAxis::Type::Y);
		//NewMesh->SetSplineUpDir(FVector(0.0f, -1.0f, 0.0f));
		NewMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
		Segments.Add(NewMesh);
	}

	

	//UStaticMeshComponent* Test = NewObject<UStaticMeshComponent>(this);
	//NewMesh->SetStaticMesh(Mesh);
}

void ADynamicSplineSection::RefreshSpline(bool Constructor)
{
	for (auto component : Segments)
	{
		component->UnregisterComponent();
		component->DestroyComponent();
	}
	Segments.Empty();

	for (int i = 0; i < Spline->GetNumberOfSplinePoints(); i++)
	{
		if(i != Spline->GetNumberOfSplinePoints() - 1)
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
		}
	}

	//auto meta = Spline->GetSplinePointsMetadata();

}


// Called when the game starts or when spawned
void ADynamicSplineSection::BeginPlay()
{
	Super::BeginPlay();
	RefreshSpline(false);
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