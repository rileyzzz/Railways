// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeActorAsset.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/DecalComponent.h"
#include "DynamicSplineSection.generated.h"

//struct SplinePointMetadata
//{
//	float SplineRoll = 0.0f;
//};

UCLASS()
class RAILWAYS_API ADynamicSplineSection : public ARuntimeActorAsset
{
	GENERATED_BODY()
	
private:
	void BuildSpline(bool Constructor);
	void BuildSplineSegment(int32 index, UStaticMesh* Mesh, bool Constructor);
	void UpdateSplineSegment(int32 index);
	int CachedSplineCount = 0;
public:
	USplineComponent* Spline;
	//TMap<int32, SplinePointMetadata> PointMetadata;
	TArray<USplineMeshComponent*> Segments;
	TArray<UDecalComponent*> Decals;

	UPROPERTY(EditAnywhere)
	UStaticMesh* SplineMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DecalMaterial;

	void EnableCollision();
	void DisableCollision();
	void RefreshSpline();

	// Sets default values for this actor's properties
	ADynamicSplineSection();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
