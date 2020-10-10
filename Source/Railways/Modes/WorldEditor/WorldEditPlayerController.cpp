// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEditPlayerController.h"
#include "Kismet/GameplayStatics.h"

AWorldEditPlayerController::AWorldEditPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//bReplicates = true;
	if (HasAuthority()) SetReplicates(true);
}

void AWorldEditPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY!"));

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHeightWorld::StaticClass(), FoundActors);
	if(FoundActors.Num()) WorldRef = Cast<AHeightWorld>(FoundActors[0]);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AWorldEditPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, TEXT("AUTHORITY"));
		TestForTileServer_Implementation();
		
	}
}

void AWorldEditPlayerController::TestForTileServer_Implementation()
{
	APawn* LocalPawn = GetPawn();
	if (LocalPawn)
	{
		FVector PlayerPos = LocalPawn->GetActorLocation() / ((float)WORLD_SIZE * WORLD_SCALE);
		int TileX = FMath::FloorToInt(PlayerPos.X);
		int TileY = FMath::FloorToInt(PlayerPos.Y);
		constexpr int BuildRadius = 4;
		for (int BuildX = TileX - BuildRadius; BuildX < TileX + BuildRadius; BuildX++)
		{
			for (int BuildY = TileY - BuildRadius; BuildY < TileY + BuildRadius; BuildY++)
			{
				WorldRef->TestForTile(BuildX, BuildY);
			}
		}
	}
}
