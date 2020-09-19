// Fill out your copyright notice in the Description page of Project Settings.


#include "RailwaysGameInstance.h"

void URailwaysGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("Init"));
}

void URailwaysGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogTemp, Warning, TEXT("Shutdown"));
}