// Fill out your copyright notice in the Description page of Project Settings.


#include "RailwaysPlayerState.h"
#include "Modes/WorldEditor/WorldEditorPawn.h"
#include "Kismet/GameplayStatics.h"


void ARailwaysPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARailwaysPlayerState, SteamID);
}

void ARailwaysPlayerState::BeginPlay()
{
	Super::BeginPlay();

	//if (HasAuthority())
	//{
	//	
	//}
}

void ARailwaysPlayerState::SetSteamID(const FSteamID& ID)
{
	//FString Message = FString::Printf(TEXT("Setting SteamID to %u for player %s"), ID.ID, *GetPlayerName());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, Message);
	SteamID = ID;

	//force replicate on server
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Force replicating on server"));
		AWorldEditorPawn* Pawn = GetPawn<AWorldEditorPawn>();
		Pawn->OnRep_PlayerState();
	}

	ForceNetUpdate();
}