// Fill out your copyright notice in the Description page of Project Settings.


#include "RailwaysGameInstance.h"

void URailwaysGameInstance::Init()
{
	Super::Init();
	//Interface = GetContentSystem();
	Interface = UContentSystemInterface::GetInterface();
	AssimpInterface = NewObject<UAssimpInterface>();
}

void URailwaysGameInstance::Shutdown()
{
	//UE_LOG(LogTemp, Warning, TEXT("Shutdown instance"));
	UContentSystemInterface::ShutdownInterface();
	Interface = nullptr;
	Super::Shutdown();
}