// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Core/RailwaysGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "RailwaysPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API ARailwaysPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated)
	FSteamID SteamID;

	void SetSteamID(const FSteamID& ID);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
