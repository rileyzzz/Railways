// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEditPlayerController.h"
#include "WorldEditorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "../../RailwaysPlayerState.h"

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
	if (UGameplayStatics::GetPlayerControllerID(this) == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("AUTHORITY"));
		//send steamID after 0.4 seconds
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AWorldEditPlayerController::SendSteamID, 0.4f, false);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("NO PLAYERCONTROLLER AUTHORITY"));
	}
	
}

void AWorldEditPlayerController::SendSteamID()
{
	URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FSteamID ID = GameInstance->getSteamID();
	//FString Message = FString::Printf(TEXT("Sending SteamID %u"), ID.ID);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, Message);
	ServerSetSteamID(ID);
}

//void AWorldEditPlayerController::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//	OnRep_PlayerState();
//}

void AWorldEditPlayerController::ServerSetSteamID_Implementation(const FSteamID& ID)
{
	//set the steam ID to be replicated on the server
	ARailwaysPlayerState* MyPlayerState = GetPlayerState<ARailwaysPlayerState>();
	MyPlayerState->SetSteamID(ID);
}

void AWorldEditPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

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
	//DOREPLIFETIME(AWorldEditPlayerController, PlayerID);
}
