// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../World/HeightWorld.h"
#include "WorldEditPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API AWorldEditPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	AHeightWorld* WorldRef;
public:
	AWorldEditPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Unreliable)
	void TestForTileServer();
	void TestForTileServer_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
