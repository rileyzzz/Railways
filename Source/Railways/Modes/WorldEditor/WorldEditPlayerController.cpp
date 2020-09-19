// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldEditPlayerController.h"

void AWorldEditPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY!"));
	
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

AWorldEditPlayerController::AWorldEditPlayerController()
{

}