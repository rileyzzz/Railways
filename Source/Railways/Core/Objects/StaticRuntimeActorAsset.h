// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeActorAsset.h"
#include "StaticRuntimeActorAsset.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API AStaticRuntimeActorAsset : public ARuntimeActorAsset
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	UProceduralMeshComponent* MeshComponent;

	void BuildMeshNode(const RailwaysNode& Node);
public:

	virtual void InitAsset() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AStaticRuntimeActorAsset();
	~AStaticRuntimeActorAsset();
};
