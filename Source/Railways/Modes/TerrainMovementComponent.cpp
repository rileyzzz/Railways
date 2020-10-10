// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainMovementComponent.h"
#include "DrawDebugHelpers.h"

//void UTerrainMovementComponent::UpdatePositionToGround(FVector& Position)
//{
//    constexpr float range = 10000.0f;
//    FVector Start = Position + FVector(0.0f, 0.0f, range);
//    FVector End = Position - FVector(0.0f, 0.0f, range);
//    FCollisionQueryParams CollisionParams;
//    FHitResult OutHit;
//    if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
//    {
//        Position = OutHit.ImpactPoint;
//    }
//}

void UTerrainMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Make sure that everything is still valid, and that we are allowed to move.
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    // Get (and then clear) the movement vector that we set in ACollidingPawn::Tick
    FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 2000.0f;

    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
        
        
        // If we bumped into something, try to slide along it
        //if (Hit.IsValidBlockingHit())
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("VALID BLOCKING HIT"));
        //    DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + Hit.ImpactNormal * 1000.0f, FColor::Green, true);
        //    //SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit, false);
        //    //MoveAlongFloor(DesiredMovementThisFrame, DeltaTime);
        //}
    }
}