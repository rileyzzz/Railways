// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WorldEditPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API AWorldEditPlayerController : public APlayerController
{
	GENERATED_BODY()

	AWorldEditPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
