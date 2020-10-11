// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "RuntimeActorAsset.generated.h"

UCLASS()
class RAILWAYS_API ARuntimeActorAsset : public AActor
{
	GENERATED_BODY()
	
private:
	//UAssimpInterface* AssimpInterface;

public:	
	// Sets default values for this actor's properties
	ARuntimeActorAsset();

	UPROPERTY(EditAnywhere)
	UMaterialInterface* PBRMaterial;

	UPROPERTY()
	UProceduralMeshComponent* DynamicMesh;

	void InitMesh();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void UpdatePositionToGround(FVector& Position);
	void UpdatePositionToGroundLocal(FVector& Position);
};
