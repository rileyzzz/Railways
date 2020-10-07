// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerListEntry.h"
#include "../RailwaysGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UServerListEntry::JoinSession_Internal(const FRailwaysServerData& Session)
{
	URailwaysGameInstance* Instance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Instance->JoinSession(Session.Index);
}