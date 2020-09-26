// Fill out your copyright notice in the Description page of Project Settings.


#include "DieselTest.h"

// Sets default values
ADieselTest::ADieselTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	DynamicMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("DynamicMesh"));
	DynamicMesh->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ADieselTest::BeginPlay()
{
	Super::BeginPlay();
	InitMesh();
}

// Called every frame
void ADieselTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

