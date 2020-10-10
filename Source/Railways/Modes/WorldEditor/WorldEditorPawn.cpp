// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldEditorPawn.h"
#include "Components/InputComponent.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWorldEditorPawn::AWorldEditorPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
	EditCursor->SetVisibility(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	SpringArm->SetIsReplicated(true);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	

	Username = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Username"));
	Username->SetupAttachment(SpringArm);
	//Username->SetFont(TextFont);

	//Username->SetText(TEXT("test"));
	Username->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	AvatarImage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AvatarImage"));
	AvatarImage->SetupAttachment(SpringArm);
	//AvatarImage->SetMobility(EComponentMobility::Movable);
	AvatarImage->SetRelativeLocation(FVector(0.0f, 30.0f, 30.0f));
	AvatarImage->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	//AvatarImage->SetStaticMesh(AvatarImageMesh);
	AvatarImage->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	

	MovementComponent = CreateDefaultSubobject<UTerrainMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	//bReplicates = true;
	//bReplicateMovement = true;
	if (HasAuthority()) SetReplicates(true);
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

void AWorldEditorPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Cursor->SetDecalMaterial(DecalMaterial);
	EditCursor->SetDecalMaterial(PaintMaterial);
}

// Called when the game starts or when spawned
void AWorldEditorPawn::BeginPlay()
{
	Super::BeginPlay();
	Controller = Cast<AWorldEditPlayerController>(GetController());

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHeightWorld::StaticClass(), FoundActors);
	if(FoundActors.Num()) WorldRef = Cast<AHeightWorld>(FoundActors[0]);

	if (HasAuthority())
	{
		//Player name
		APlayerState* State = GetPlayerState();
		if (State)
		{
			PlayerName = State->GetPlayerName();
			OnRep_NameText();
		}
		URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		PlayerID = GameInstance->getSteamID();
		OnRep_SteamID();

		ForceNetUpdate();
	}

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMat(TEXT("/Game/Railways/Core/Meshes/Avatar/AvatarMaterial"));
	AvatarMaterial = UMaterialInstanceDynamic::Create(AvatarImage->GetMaterial(0), this);
	AvatarImage->SetMaterial(0, AvatarMaterial);
}

void AWorldEditorPawn::OnRep_NameText()
{
	Username->SetText(PlayerName);
}

void AWorldEditorPawn::OnRep_SteamID()
{
	URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultAvatar(TEXT("/Game/Railways/Core/Modes/Menu/UI/Assets/default_icon"));
	AvatarTexture = GameInstance->getPlayerSteamAvatar(PlayerID, nullptr);
	if(AvatarTexture) AvatarMaterial->SetTextureParameterValue(FName(TEXT("AvatarImage")), AvatarTexture);
}

// Called every frame
void AWorldEditorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const float ZoomFactor = 1.0f + (SpringArm->TargetArmLength / 4000.0f);
	//UE_LOG(LogTemp, Log, TEXT("zoom factor %f"), ZoomFactor);
	//FVector Forward = Camera->GetForwardVector();
	//FVector Right = Camera->GetRightVector();
	//Forward.Z = 0.0f;
	//Right.Z = 0.0f;
	//Forward.Normalize();
	//Right.Normalize();

	//Forward *= ForwardVelocity;
	//Right *= RightVelocity;

	//FVector Velocity = Forward + Right;
	////Velocity.Normalize();
	//Velocity *= (DeltaTime * 1000.0f * ZoomFactor);

	////ServerMovementUpdate(Velocity);
	//FVector NewLocation = GetActorLocation() + Velocity;
	//UpdatePositionToGround(NewLocation);
	//SetActorLocation(NewLocation);
	
	FVector Height = RootComponent->GetComponentLocation();
	UpdatePositionToGround(Height);
	RootComponent->SetWorldLocation(Height);

	//if (GEngine->GetNetMode(GetWorld()) == NM_Client || NM_Standalone)
	if (Controller)
	{
		FHitResult Target;
		if (GetMouseHit(Target))
		{
			if (EditCategory == 0)
			{
				//AHeightWorld* CurrentWorld = Cast<AHeightWorld>(Target.Actor.Get());
				//if (b_leftMouse && (EditMode == 0 || EditMode == 1))
				//{
				//	if (CurrentWorld)
				//	{
				//		auto SortedTiles = CurrentWorld->GetSortedTilesToPoint(Target.ImpactPoint);
				//		const int TileCount = FMath::Min(SortedTiles.Num(), 4);
				//		for (int i = 0; i < TileCount; i++)
				//			SortedTiles[i]->TerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 200);
				//	}
				//}
				//else if (b_leftMouse && (EditMode == 2))
				//{
				//	if (CurrentWorld)
				//	{
				//		auto SortedTiles = CurrentWorld->GetSortedTilesToPoint(Target.ImpactPoint);
				//		const int TileCount = (SortedTiles.Num() < 4) ? SortedTiles.Num() : 4;
				//		for (int i = 0; i < TileCount; i++)
				//			SortedTiles[i]->TerrainApproach(Target.ImpactPoint, TargetHeight, 0.4f, 200);
				//	}
				//}
				AActor* Actor = Target.Actor.Get();

				if (Actor)
				{
					//AHeightWorld* CurrentWorld = Cast<AHeightWorld>(Actor->GetAttachParentActor());
					AHeightWorld* CurrentWorld = WorldRef;
					if (b_leftMouse && (EditMode == 0 || EditMode == 1))
					{
						//UE_LOG(LogTemp, Warning, TEXT("pre influence"));
						if (CurrentWorld)
						{
							/*auto SortedTiles = CurrentWorld->GetSortedTilesToPoint(Target.ImpactPoint);
							const int TileCount = FMath::Min(SortedTiles.Num(), 4);
							for (int i = 0; i < TileCount; i++)
								SortedTiles[i]->TerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 200);*/
							//UE_LOG(LogTemp, Warning, TEXT("calling influence"));
							//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, TEXT("Calling"));
							ServerTerrainInfluence(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 200);
							//ServerTerrainInfluence_Implementation(Target.ImpactPoint, EditMode == 0 ? 1.0f : -1.0f, 200);
						}
					}
					else if (b_leftMouse && (EditMode == 2))
					{
						if (CurrentWorld)
						{
							ServerTerrainApproach(Target.ImpactPoint, TargetHeight, 0.4f, 200);
						}
					}
				}
			}
			else if (EditCategory == 1)
			{
				if (b_leftMouse)
				{
					if (EditSplinePoint)
					{
						EditSplinePoint->SetWorldLocation(Target.ImpactPoint);
						EditSplinePoint->ParentSection->RefreshSection();
					}
				}
			}

			//FTransform transform = GetTransform();
			if (EditCursor) EditCursor->SetWorldLocation(Target.ImpactPoint);
		}
	}
}

// Called to bind functionality to input
void AWorldEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Fly Forward", this, &AWorldEditorPawn::InputMoveForward);
	InputComponent->BindAxis("Fly Right", this, &AWorldEditorPawn::InputMoveRight);

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

void AWorldEditorPawn::InputMoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		ServerMoveForward(AxisValue);
		ServerMoveForward_Implementation(AxisValue);
	}
		
}

void AWorldEditorPawn::InputMoveRight(float AxisValue)
{
	if (AxisValue != 0)
	{
		ServerMoveRight(AxisValue);
		ServerMoveRight_Implementation(AxisValue);
	}
}

void AWorldEditorPawn::ServerMoveForward_Implementation(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		const float ZoomFactor = 1.0f + (SpringArm->TargetArmLength / 4000.0f);
		FVector Forward = Camera->GetForwardVector();
		Forward.Z = 0.0f;
		Forward.Normalize();

		Forward *= AxisValue * ZoomFactor;
		MovementComponent->AddInputVector(Forward);
	}
}

void AWorldEditorPawn::ServerMoveRight_Implementation(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		const float ZoomFactor = 1.0f + (SpringArm->TargetArmLength / 4000.0f);
		FVector Right = Camera->GetRightVector();
		Right.Z = 0.0f;
		Right.Normalize();

		Right *= AxisValue * ZoomFactor;
		MovementComponent->AddInputVector(Right);
	}
}

void AWorldEditorPawn::InputCameraX(float AxisValue)
{
	if (b_draggingMouse)
	{
		ServerCameraX(AxisValue);
		ServerCameraX_Implementation(AxisValue);
	}
}

void AWorldEditorPawn::InputCameraY(float AxisValue)
{
	if (b_draggingMouse)
	{
		ServerCameraY(AxisValue);
		ServerCameraY_Implementation(AxisValue);
	}
}

void AWorldEditorPawn::InputCameraZoom(float AxisValue)
{
	ServerCameraZoom(AxisValue);
	ServerCameraZoom_Implementation(AxisValue);
}

void AWorldEditorPawn::ServerCameraZoom_Implementation(float AxisValue)
{
	ClientCameraZoom(AxisValue);
}

void AWorldEditorPawn::ClientCameraZoom_Implementation(float AxisValue)
{
	float Target = SpringArm->TargetArmLength;
	Target /= (AxisValue / 4.0f) + 1.0f;
	//Target /= AxisValue * 40.0f;
	if (Target > 10.0f && Target <= 16000.0f)
		SpringArm->TargetArmLength = Target;
}

void AWorldEditorPawn::ServerCameraX_Implementation(float AxisValue)
{
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	CurrentRotation.Yaw += AxisValue * 2.0f;
	CurrentRotation.Roll = 0.0f;
	SpringArm->SetRelativeRotation(CurrentRotation);
}

void AWorldEditorPawn::ServerCameraY_Implementation(float AxisValue)
{
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	CurrentRotation.Pitch += AxisValue * 2.0f;
	CurrentRotation.Roll = 0.0f;

	//UE_LOG(LogTemp, Warning, TEXT("Pitch %f"), CurrentRotation.Pitch);
	if (CurrentRotation.Pitch < -1.0f && CurrentRotation.Pitch > -89.0f)
		SpringArm->SetRelativeRotation(CurrentRotation);
}

void AWorldEditorPawn::ServerTerrainInfluence_Implementation(const FVector_NetQuantize100& HitPoint, float Direction, int Radius)
{
	//Executes on server
	auto SortedTiles = WorldRef->GetSortedTilesToPoint(HitPoint);
	const int TileCount = FMath::Min(SortedTiles.Num(), 4);
	for (int i = 0; i < TileCount; i++)
		SortedTiles[i]->TerrainInfluence(HitPoint, Direction, Radius);
}

void AWorldEditorPawn::ServerTerrainApproach_Implementation(const FVector_NetQuantize100& HitPoint, float Height, float Strength, int Radius)
{
	//Executes on server
	auto SortedTiles = WorldRef->GetSortedTilesToPoint(HitPoint);
	const int TileCount = (SortedTiles.Num() < 4) ? SortedTiles.Num() : 4;
	for (int i = 0; i < TileCount; i++)
		SortedTiles[i]->TerrainApproach(HitPoint, Height, Strength, Radius);
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
			EditSplineDummy = dynamic_cast<USphereComponent*>(Target.Component.Get());
			if (EditSplineDummy)
			{
				UE_LOG(LogTemp, Warning, TEXT("sphere found"));
				UDynamicSplinePoint* HitPoint = dynamic_cast<UDynamicSplinePoint*>(EditSplineDummy->GetAttachParent());
				if (HitPoint)
				{
					UE_LOG(LogTemp, Warning, TEXT("point found"));
					if (EditMode == 0)
					{
						UDynamicSplinePoint* NewPoint = NewObject<UDynamicSplinePoint>(HitPoint->ParentSection, UDynamicSplinePoint::StaticClass(), NAME_None);
						NewPoint->ParentSection = HitPoint->ParentSection;
						NewPoint->Mesh = HitPoint->ParentSection->SplineMesh;
						NewPoint->RegisterComponent();
						NewPoint->SetMobility(EComponentMobility::Movable);
						NewPoint->AttachToComponent(HitPoint->ParentSection->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
						NewPoint->SetWorldLocation(Target.ImpactPoint);

						HitPoint->Paths.Add(NewPoint);
						NewPoint->Paths.Add(HitPoint);


						if (HitPoint == HitPoint->ParentSection->RootPoint) HitPoint->ParentSection->RootPoint = NewPoint; //beginning

						HitPoint->ParentSection->RootPoint->RootBuild();

						EditSplinePoint = NewPoint;
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
	UE_LOG(LogTemp, Warning, TEXT("end mouse"));
	//join points
	if (EditSplinePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("tracing"));
		TArray<UPrimitiveComponent*> IgnoreComponents;
		IgnoreComponents.Add(EditSplineDummy);
		FHitResult Target;
		if (GetMouseHit(Target, ECollisionChannel::ECC_GameTraceChannel1, IgnoreComponents))
		{
			if (EditCategory == 1)
			{
				USphereComponent* dummy = dynamic_cast<USphereComponent*>(Target.Component.Get());
				if (dummy)
				{
					UE_LOG(LogTemp, Warning, TEXT("other sphere found"));
					UDynamicSplinePoint* HitPoint = dynamic_cast<UDynamicSplinePoint*>(dummy->GetAttachParent());
					if (HitPoint && HitPoint != EditSplinePoint)
					{
						UE_LOG(LogTemp, Warning, TEXT("other point found"));
						if (EditMode == 0 || EditMode == 2)
						{
							/*if (!EditSplinePoint->Paths.Contains(HitPoint) && !HitPoint->Paths.Contains(EditSplinePoint))
							{
								EditSplinePoint->Paths.Add(HitPoint);
								HitPoint->Paths.Add(EditSplinePoint);
							}*/
							for (auto& segment : HitPoint->ParentSection->Segments)
							{
								auto& Start = segment.StartPoint;
								auto& End = segment.EndPoint;

								for (auto& path : Start->Paths)
								{
									if (path == EditSplinePoint)
										path = HitPoint;
								}

								for (auto& path : End->Paths)
								{
									if (path == EditSplinePoint)
										path = HitPoint;
								}
							}

							EditSplinePoint->UnregisterComponent();
							EditSplinePoint->DestroyComponent();

							//EditSplinePoint = HitPoint;

							//if (HitPoint == HitPoint->ParentSection->RootPoint) HitPoint->ParentSection->RootPoint = NewPoint; //beginning

							HitPoint->ParentSection->RootPoint->RootBuild();
						}
					}
				}
			}
		}
	}

	//if(EditSplineSection) EditSplineSection->EnableCollision();
	b_leftMouse = false;
	EditSpline = nullptr;
	EditSplineSection = nullptr;
	EditSplinePoint = nullptr;
	EditSplineDummy = nullptr;
}

bool AWorldEditorPawn::GetMouseHit(FHitResult& OutHit, ECollisionChannel channel, const TArray<UPrimitiveComponent*>& Ignore)
{
	//UE_LOG(LogTemp, Warning, TEXT("tracing line"));
	FVector Position, Direction;
	Controller->DeprojectMousePositionToWorld(Position, Direction);
	FVector End = Position + Direction * 40000.0f;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredComponents(Ignore);
	return GetWorld()->LineTraceSingleByChannel(OutHit, Position, End, channel, CollisionParams);
}

