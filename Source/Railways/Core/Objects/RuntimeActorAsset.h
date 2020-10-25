// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
//#include "ProceduralSkeletalMeshComponent.h"
#include "AssimpInterface.h"
#include "MeshContent.h"
#include "RuntimeActorAsset.generated.h"

UCLASS()
class RAILWAYS_API ARuntimeActorAsset : public AActor
{
	GENERATED_BODY()
	
private:

	//retrieve content information from content system
	void LoadContentData();

	
public:
	// Sets default values for this actor's properties
	ARuntimeActorAsset();

	FMeshContent MeshContent;

	UPROPERTY()
	TMap<FString, UTexture2D*> TextureCache;

	UTexture2D* LoadTextureFile(FString Path, bool SRGB = true);

	UPROPERTY()
	uint64 ContentID;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* PBRMaterial;

	//UPROPERTY()
	//UProceduralSkeletalMeshComponent* DynamicMesh;

	//Called automatically, after all relevant content data has been loaded
	UFUNCTION()
	virtual void InitAsset();

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
