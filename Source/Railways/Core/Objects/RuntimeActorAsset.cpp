// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeActorAsset.h"

// Sets default values
ARuntimeActorAsset::ARuntimeActorAsset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARuntimeActorAsset::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARuntimeActorAsset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

