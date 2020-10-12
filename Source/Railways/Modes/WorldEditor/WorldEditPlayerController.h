// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../World/HeightWorld.h"
#include "../../Core/RailwaysGameInstance.h"
#include "WorldEditPlayerController.generated.h"

class AWorldEditorPawn;

UCLASS()
class RAILWAYS_API AWorldEditPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AHeightWorld* WorldRef;

public:
	AWorldEditPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Unreliable)
	void TestForTileServer();
	void TestForTileServer_Implementation();

	UFUNCTION()
	void SendSteamID();

	UFUNCTION(Server, Reliable)
	void ServerSetSteamID(const FSteamID& ID);
	void ServerSetSteamID_Implementation(const FSteamID& ID);

	virtual void OnRep_PlayerState() override;

	//virtual void PostInitializeComponents() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
