// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldEditorPawn.h"
#include "Components/InputComponent.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWorldEditorPawn::AWorldEditorPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(LogTemp, Warning, TEXT("actor constructor"));

	

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Cursor = CreateDefaultSubobject<UDecalComponent>(TEXT("Cursor"));
	Cursor->SetDecalMaterial(DecalMaterial);
	Cursor->SetupAttachment(RootComponent);
	Cursor->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
	Cursor->DecalSize = FVector(128.0f, 128.0f, 128.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	//Camera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	//Camera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
}

#if UE_BUILD_DEBUG
void AWorldEditorPawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Cursor->SetDecalMaterial(DecalMaterial);
}
#endif

void AWorldEditorPawn::SetEditMode(int mode)
{
	UE_LOG(LogTemp, Warning, TEXT("edit mode changed to %i"), mode);
	EditMode = mode;
}

// Called when the game starts or when spawned
void AWorldEditorPawn::BeginPlay()
{
	Super::BeginPlay();
	Controller = dynamic_cast<AWorldEditPlayerController*>(GetController());

	Cursor->SetDecalMaterial(DecalMaterial);
}

// Called every frame
void AWorldEditorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("tick"));
	//if (!Velocity.IsZero())
	//{
	//	FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
	//	SetActorLocation(NewLocation);
	//}
	if (CurrentEditingTile)
	{
		switch (EditMode)
		{
		case 0:
		{
			CurrentEditingTile->TerrainInfluence(LastHit, 1.0, 800);
			break;
		}
		default:
		case -1:
			break;
		}
	}
}

// Called to bind functionality to input
void AWorldEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Fly Forward", this, &AWorldEditorPawn::InputFlyForward);
	InputComponent->BindAxis("Fly Right", this, &AWorldEditorPawn::InputFlyRight);

	InputComponent->BindAxis("Rotate Camera X", this, &AWorldEditorPawn::InputCameraX);
	InputComponent->BindAxis("Rotate Camera Y", this, &AWorldEditorPawn::InputCameraY);
	InputComponent->BindAxis("Zoom", this, &AWorldEditorPawn::InputCameraZoom);

	InputComponent->BindAction("Drag Camera", IE_Pressed, this, &AWorldEditorPawn::StartDrag);
	InputComponent->BindAction("Drag Camera", IE_Released, this, &AWorldEditorPawn::EndDrag);

	InputComponent->BindAction("Left Mouse", IE_Pressed, this, &AWorldEditorPawn::StartMouse);
	InputComponent->BindAction("Left Mouse", IE_Released, this, &AWorldEditorPawn::EndMouse);
}

void AWorldEditorPawn::UpdatePositionToGround(FVector& Position)
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

void AWorldEditorPawn::InputFlyForward(float AxisValue)
{
	//Velocity += Camera->GetForwardVector() * AxisValue * 1.0f;
	FVector Forward = Camera->GetForwardVector() * AxisValue;
	Forward.Z = 0;
	Forward.Normalize();
	Forward *= 10.0f;
	FVector NewLocation = GetActorLocation() + Forward;
	UpdatePositionToGround(NewLocation);
	SetActorLocation(NewLocation);

	//Velocity.X = AxisValue * 800.0f;
	//FVector translation(AxisValue, 0.0f, 0.0f);
	//AddActorWorldTransform(FTransform(translation), false);
}

void AWorldEditorPawn::InputFlyRight(float AxisValue)
{
	//Velocity += Camera->GetRightVector() * AxisValue * 1.0f;

	FVector Right = Camera->GetRightVector() * AxisValue;
	Right.Z = 0;
	Right.Normalize();
	Right *= 10.0f;
	FVector NewLocation = GetActorLocation() + Right;
	UpdatePositionToGround(NewLocation);
	SetActorLocation(NewLocation);

	//FVector translation(0.0f, AxisValue, 0.0f);
	//AddActorWorldTransform(FTransform(translation), false);
}

void AWorldEditorPawn::InputCameraX(float AxisValue)
{
	if (b_draggingMouse)
	{
		FRotator CurrentRotation = SpringArm->GetRelativeRotation();
		CurrentRotation.Yaw += AxisValue * 2.0f;
		CurrentRotation.Roll = 0.0f;
		SpringArm->SetRelativeRotation(CurrentRotation);
	}
}

void AWorldEditorPawn::InputCameraY(float AxisValue)
{
	if (b_draggingMouse)
	{
		FRotator CurrentRotation = SpringArm->GetRelativeRotation();
		CurrentRotation.Pitch += AxisValue * 2.0f;
		CurrentRotation.Roll = 0.0f;
		
		//UE_LOG(LogTemp, Warning, TEXT("Pitch %f"), CurrentRotation.Pitch);
		if(CurrentRotation.Pitch < -1.0f && CurrentRotation.Pitch > -89.0f)
			SpringArm->SetRelativeRotation(CurrentRotation);
	}
}

void AWorldEditorPawn::InputCameraZoom(float AxisValue)
{
	float Target = SpringArm->TargetArmLength;
	Target /= (AxisValue / 4.0f) + 1.0f;
	//Target /= AxisValue * 40.0f;
	if (Target > 10.0f && Target <= 8000.0f)
		SpringArm->TargetArmLength = Target;
}

void AWorldEditorPawn::StartDrag()
{
	b_draggingMouse = true;
}

void AWorldEditorPawn::EndDrag()
{
	b_draggingMouse = false;
}

bool AWorldEditorPawn::GetMouseHit(FHitResult& OutHit)
{
	UE_LOG(LogTemp, Warning, TEXT("tracing line"));
	FVector Position, Direction;
	Controller->DeprojectMousePositionToWorld(Position, Direction);
	FVector End = Position + Direction * 10000.0f;
	FCollisionQueryParams CollisionParams;
	return GetWorld()->LineTraceSingleByChannel(OutHit, Position, End, ECC_Visibility, CollisionParams);

}

void AWorldEditorPawn::StartMouse()
{
	FHitResult Target;
	if (GetMouseHit(Target))
	{
		LastHit = Target.ImpactPoint;
		CurrentEditingTile = dynamic_cast<UWorldTileDynamic*>(Target.Component.Get());
		if (CurrentEditingTile)
		{
			DrawDebugLine(GetWorld(), Target.TraceStart, Target.ImpactPoint, FColor::Green, true);
			DrawDebugLine(GetWorld(), Target.ImpactPoint, Target.ImpactPoint + Target.ImpactNormal * 100.0f, FColor::Blue, true);
		}
	}
}

void AWorldEditorPawn::EndMouse()
{
	CurrentEditingTile = nullptr;
	switch (EditMode)
	{
	case 0:
		break;
	default:
	case -1:
		break;
	}
}

