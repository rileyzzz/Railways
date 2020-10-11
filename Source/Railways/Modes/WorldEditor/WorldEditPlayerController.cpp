// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEditPlayerController.h"
#include "WorldEditorPawn.h"
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


	//PawnRef = Cast<AWorldEditorPawn>(GetPawn());

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("has authority"));

		
		//needs to be in here? pawn doesnt have authority for some reason
		URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		PlayerID = GameInstance->getSteamID();
		
		//PawnRef->ForceNetUpdate();
		
	}

	ServerSetupClientData_Implementation();
}

//void AWorldEditPlayerController::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//	OnRep_PlayerState();
//}

void AWorldEditPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	UE_LOG(LogTemp, Warning, TEXT("player state rep"));
	if (PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("player state exists"));
		//UE_LOG(LogTemp, Warning, TEXT("has state %s"), *PlayerState->GetPlayerName());
		//PlayerName = PlayerState->GetPlayerName();
		//OnRep_NameText();
		ServerSetupClientData();
	}
}

void AWorldEditPlayerController::OnRep_SteamID()
{
	AWorldEditorPawn* PawnRef = Cast<AWorldEditorPawn>(GetPawn());
	if (!HasAuthority()) //server?
	{
		PawnRef->PlayerID = PlayerID;
	}
}

void AWorldEditPlayerController::ServerSetupClientData_Implementation()
{
	//Executed on server
	UE_LOG(LogTemp, Warning, TEXT("Setting up server data"));
	AWorldEditorPawn* PawnRef = Cast<AWorldEditorPawn>(GetPawn());
	//Player name
	if (PawnRef)
	{
		if (PlayerState)
		{
			PawnRef->PlayerName = PlayerState->GetPlayerName();
			//UE_LOG(LogTemp, Warning, TEXT("Player name %s"), *PlayerName);
			PawnRef->OnRep_NameText();
		}

		PawnRef->ForceNetUpdate();
	}
}

void AWorldEditPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, TEXT("AUTHORITY"));
		TestForTileServer();
		//TestForTileServer_Implementation();
		
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

void AWorldEditPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(AWorldEditPlayerController, PlayerName);
	DOREPLIFETIME(AWorldEditPlayerController, PlayerID);
}
