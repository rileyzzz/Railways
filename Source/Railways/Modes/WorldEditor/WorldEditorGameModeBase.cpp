// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEditorGameModeBase.h"

void AWorldEditorGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	//GenericPlayerInitialization(NewPlayer);

	//NewPlayer->ClientCapBandwidth(NewPlayer->Player->CurrentNetSpeed);


	//// Notify Blueprints that a new player has logged in.  Calling it here, because this is the first time that the PlayerController can take RPCs
	////K2_PostLogin(NewPlayer);


	//FActorSpawnParameters SpawnParams;
	//AWorldEditorPawn* NewPawn = GetWorld()->SpawnActor<AWorldEditorPawn>(DefaultPawnClass, SpawnParams);
	//NewPlayer->Possess(NewPawn);
	//NewPawn->CopyRemoteRoleFrom(NewPlayer);
}