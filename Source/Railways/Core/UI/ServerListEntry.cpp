// Fill out your copyright notice in the Description page of Project Settings.

#include "../RailwaysGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "ServerListEntry.h"

void UServerListEntry::JoinSession_Internal(const FRailwaysServerData& Session)
{
	URailwaysGameInstance* Instance = Cast<URailwaysGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Instance->JoinSession(Session.Index);
}