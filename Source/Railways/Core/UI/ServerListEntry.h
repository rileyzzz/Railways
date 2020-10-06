// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ServerListEntry.generated.h"

USTRUCT(BlueprintType)
struct FRailwaysServerData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "ServerData")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "ServerData")
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "ServerData")
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "ServerData")
	FString HostUsername;
};

UCLASS()
class RAILWAYS_API UServerListEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	//UPROPERTY(BlueprintReadWrite)
	//FRailwaysServerData ServerListEntryData;
	//UPROPERTY(BlueprintReadWrite)
	//bool TestServerBool = false;
	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//UTextBlock* ServerName;

	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//UTextBlock* PlayerCountDetails;

	//UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	//FRailwaysServerData ServerData;
};
