// Fill out your copyright notice in the Description page of Project Settings.


#include "ContentTests.h"

// Sets default values
AContentTests::AContentTests()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Skeleton = CreateDefaultSubobject<UProceduralSkeletalMeshComponent>(TEXT("skeleton"));
	Skeleton->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AContentTests::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Beginning Assimp"));
	Interface = NewObject<UAssimpInterface>();
	auto test = Interface->ImportFBX();
	Interface->BuildComponent(Skeleton, test);
}

// Called every frame
void AContentTests::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

