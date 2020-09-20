// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"

// Sets default values
ARuntimeActorAsset::ARuntimeActorAsset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARuntimeActorAsset::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARuntimeActorAsset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARuntimeActorAsset::UpdatePositionToGround(FVector& Position)
{
	constexpr float range = 10000.0f;
	FVector Start = Position + FVector(0.0f, 0.0f, range);
	FVector End = Position - FVector(0.0f, 0.0f, range);
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		Position = OutHit.ImpactPoint;
	}
}

void ARuntimeActorAsset::UpdatePositionToGroundLocal(FVector& Position)
{
	FTransform Transform = GetTransform();
	FVector WorldTransform = Transform.GetLocation() + Position;
	UpdatePositionToGround(WorldTransform);
	Position = Transform.InverseTransformPosition(WorldTransform);
}