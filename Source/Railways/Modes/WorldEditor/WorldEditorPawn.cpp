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

	EditCursor = CreateDefaultSubobject<UDecalComponent>(TEXT("EditCursor"));
	EditCursor->SetDecalMaterial(PaintMaterial);
	EditCursor->SetupAttachment(RootComponent);
	EditCursor->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
	EditCursor->DecalSize = FVector(512.0f, 800.0f, 800.0f);
	EditCursor->SetVisibility(true);

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
	EditCursor->SetDecalMaterial(PaintMaterial);
}
#endif

void AWorldEditorPawn::SetEditCategory(int category)
{
	UE_LOG(LogTemp, Warning, TEXT("edit category changed to %i"), category);
	EditCategory = category;
	EditCursor->SetVisibility(false);
}

void AWorldEditorPawn::SetEditMode(int mode)
{
	UE_LOG(LogTemp, Warning, TEXT("edit mode changed to %i"), mode);
	EditMode = mode;

	if (EditCategory == 0)
	{
		switch (mode)
		{
		case 0:
		case 1:
		case 2:
			EditCursor->SetVisibility(true);
			break;
		default:
		case -1:
			EditCursor->SetVisibility(false);
			break;
		}
	}
}

void AWorldEditorPawn::SetTargetHeight(float height)
{
	TargetHeight = height;
}

// Called when the game starts or when spawned
void AWorldEditorPawn::BeginPlay()
{
	Super::BeginPlay();
	Controller = dynamic_cast<AWorldEditPlayerController*>(GetController());

	Cursor->SetDecalMaterial(DecalMaterial);
	EditCursor->SetDecalMaterial(PaintMaterial);
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
	/*if (CurrentEditingTile)
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
	}*/
	FHitResult Target;
	if (GetMouseHit(Target))
	{
		if (EditCategory == 0)
		{
			AHeightWorld* CurrentWorld = dynamic_cast<AHeightWorld*>(Target.Actor.Get());
			if (b_leftMouse && (EditMode == 0 || EditMode == 1))
			{
				if (CurrentWorld)
				{
					auto SortedTiles = CurrentWorld->GetSortedTilesToPoint(Target.ImpactPoint);
					const int TileCount = (SortedTiles.Num() < 4) ? SortedTiles.Num() : 4;
					for (int i = 0; i < TileCount; i++)
						SortedTiles[i]->TerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 200);

					/*for (auto& tile : CurrentWorld->Tiles)
					{
						tile->TerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 800);
					}*/
				}
				/*UWorldTileDynamic* CurrentEditingTile = dynamic_cast<UWorldTileDynamic*>(Target.Component.Get());

				if (CurrentEditingTile)
				{
					CurrentEditingTile->TerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 800);
					DrawDebugLine(GetWorld(), Target.TraceStart, Target.ImpactPoint, FColor::Green, false);
					DrawDebugLine(GetWorld(), Target.ImpactPoint, Target.ImpactPoint + Target.ImpactNormal * 100.0f, FColor::Blue, false);
				}*/
				
			}
			else if (b_leftMouse && (EditMode == 2))
			{
				if (CurrentWorld)
				{
					auto SortedTiles = CurrentWorld->GetSortedTilesToPoint(Target.ImpactPoint);
					const int TileCount = (SortedTiles.Num() < 4) ? SortedTiles.Num() : 4;
					for (int i = 0; i < TileCount; i++)
						SortedTiles[i]->TerrainApproach(Target.ImpactPoint, TargetHeight, 0.4f, 200);
				}
			}
		}
		else if (EditCategory == 1)
		{
			if (b_leftMouse)
			{
				if (EditSplinePoint)
				{
					UE_LOG(LogTemp, Warning, TEXT("setting world location"));
					EditSplinePoint->SetWorldLocation(Target.ImpactPoint);
					EditSplinePoint->ParentSection->RefreshSection();
				}
			}
		}

		//FTransform transform = GetTransform();
		if(EditCursor) EditCursor->SetWorldLocation(Target.ImpactPoint);
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

void AWorldEditorPawn::StartMouse()
{
	FHitResult Target;
	if (GetMouseHit(Target, ECollisionChannel::ECC_GameTraceChannel1))
	{
		if (EditCategory == 1)
		{
			USphereComponent* dummy = dynamic_cast<USphereComponent*>(Target.Component.Get());
			if (dummy)
			{
				UE_LOG(LogTemp, Warning, TEXT("sphere found"));
				UDynamicSplinePoint* HitPoint = dynamic_cast<UDynamicSplinePoint*>(dummy->GetAttachParent());
				if (HitPoint)
				{
					UE_LOG(LogTemp, Warning, TEXT("point found"));
					if (EditMode == 0)
					{

					}
					else
					{
						EditSplinePoint = HitPoint;
					}
				}
				//EditSplineSection = dynamic_cast<ADynamicSplineSection*>(dummy->GetAttachmentRootActor());
				//if (EditSplineSection)
				//{
				//	EditSpline = EditSplineSection->Spline;
				//	int HitIndex = FMath::RoundToInt(EditSpline->FindInputKeyClosestToWorldLocation(Target.ImpactPoint));
				//	DynamicSplinePoint* HitPoint = &EditSplineSection->Points[HitIndex];

				//	if (EditMode == 0)
				//	{
				//		int NewIndex = HitIndex;
				//		if (NewIndex == EditSpline->GetNumberOfSplinePoints() - 1) NewIndex++;
				//		if (NewIndex == 0 || NewIndex == EditSpline->GetNumberOfSplinePoints())
				//		{
				//			//create new at end
				//			EditSpline->AddSplinePointAtIndex(Target.ImpactPoint, NewIndex, ESplineCoordinateSpace::World);
				//			EditSplineSection->BuildSpline();

				//			//change point to new nearest
				//			EditSplinePoint = &EditSplineSection->Points[FMath::RoundToInt(EditSpline->FindInputKeyClosestToWorldLocation(Target.ImpactPoint))];
				//		}
				//		else
				//		{
				//			//junction
				//			FActorSpawnParameters SpawnParams;
				//			ADynamicSplineSection* NewSection = GetWorld()->SpawnActor<ADynamicSplineSection>(SpawnParams);
				//			NewSection->SetActorLocation(HitPoint->GetWorldLocation());
				//			
				//			NewSection->SplineMesh = EditSplineSection->SplineMesh;
				//			NewSection->DecalMaterial = EditSplineSection->DecalMaterial;

				//			/*HitPoint->JunctionSection = NewSection;
				//			NewSection->Points[0]->JunctionSection = EditSplineSection;*/
				//			

				//			EditSplineSection = NewSection;
				//			EditSpline = EditSplineSection->Spline;
				//			EditSplineSection->BuildSpline();
				//			EditSplinePoint = &EditSplineSection->Points[1];
				//			HitPoint->CreateJunction(&EditSplineSection->Points[0]);
				//			if (!HitPoint->JunctionSection)
				//			{
				//				UE_LOG(LogTemp, Warning, TEXT("SOURCE JUNCTION NONEXISTENT"));
				//			}
				//			else if (!EditSplineSection->Points[0].JunctionSection)
				//			{
				//				UE_LOG(LogTemp, Warning, TEXT("OUTER JUNCTION NONEXISTENT"));
				//			}
				//			else
				//			{
				//				UE_LOG(LogTemp, Warning, TEXT("JUNCTION CREATED SUCCESSFULLY"));
				//			}

				//			//EditSplineSection->BranchJunction(HitPoint, &EditSplineSection->Points[0]);
				//			
				//		}
				//	}
				//	else
				//	{
				//		EditSplinePoint = HitPoint;
				//	}
				//	

				//	UE_LOG(LogTemp, Warning, TEXT("SPLINE HIT INITIAL %i"), EditSplinePoint);
				//	//EditSplineSection->DisableCollision();
				//}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("SPLINE FAIL"));
			}
		}
	}
	b_leftMouse = true;
}

void AWorldEditorPawn::EndMouse()
{
	//if(EditSplineSection) EditSplineSection->EnableCollision();
	b_leftMouse = false;
	EditSpline = nullptr;
	EditSplineSection = nullptr;
	EditSplinePoint = nullptr;
}

bool AWorldEditorPawn::GetMouseHit(FHitResult& OutHit, ECollisionChannel channel)
{
	//UE_LOG(LogTemp, Warning, TEXT("tracing line"));
	FVector Position, Direction;
	Controller->DeprojectMousePositionToWorld(Position, Direction);
	FVector End = Position + Direction * 10000.0f;
	FCollisionQueryParams CollisionParams;
	return GetWorld()->LineTraceSingleByChannel(OutHit, Position, End, channel, CollisionParams);
}

