// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
#include "WorldEditPlayerController.h"
#include "../../World/HeightWorld.h"
#include "../../World/WorldTileDynamic.h"
#include "../../Core/Objects/DynamicSplineSection.h"
#include "../TerrainMovementComponent.h"
#include "WorldEditorPawn.generated.h"

UCLASS()
class RAILWAYS_API AWorldEditorPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWorldEditorPawn();
	virtual void PostInitializeComponents() override;

	//UPROPERTY(EditAnywhere)
	//UStaticMeshComponent* MainMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* PaintMaterial;

	UPROPERTY(BlueprintReadWrite)
	UDecalComponent* Cursor;

	UPROPERTY(BlueprintReadWrite)
	UDecalComponent* EditCursor;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	UTextRenderComponent* Username;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool b_draggingMouse = false;
	bool b_leftMouse = false;

	AHeightWorld* WorldRef;

	//THIS IS CLIENT ONLY
	AWorldEditPlayerController* Controller;

	//float ForwardVelocity = 0.0f;
	//float RightVelocity = 0.0f;
	UPROPERTY(EditAnywhere)
	UTerrainMovementComponent* MovementComponent;

	void UpdatePositionToGround(FVector& Position);
	void InputMoveForward(float AxisValue);
	void InputMoveRight(float AxisValue);

	UFUNCTION(Server, Reliable)
	void ServerMoveForward(float AxisValue);
	void ServerMoveForward_Implementation(float AxisValue);

	UFUNCTION(Server, Reliable)
	void ServerMoveRight(float AxisValue);
	void ServerMoveRight_Implementation(float AxisValue);

	UFUNCTION(Server, Reliable)
	void SetNameText(const FString& Name);
	void SetNameText_Implementation(const FString& Name);

	void InputCameraX(float AxisValue);
	void InputCameraY(float AxisValue);

	UFUNCTION(Server, Reliable)
	void ServerCameraX(float AxisValue);
	void ServerCameraX_Implementation(float AxisValue);

	UFUNCTION(Server, Reliable)
	void ServerCameraY(float AxisValue);
	void ServerCameraY_Implementation(float AxisValue);

	UFUNCTION(Server, Unreliable)
	void ServerTerrainInfluence(const FVector_NetQuantize100& HitPoint, float Direction, int Radius);
	void ServerTerrainInfluence_Implementation(const FVector_NetQuantize100& HitPoint, float Direction, int Radius);

	UFUNCTION(Server, Unreliable)
	void ServerTerrainApproach(const FVector_NetQuantize100& HitPoint, float Height, float Strength, int Radius);
	void ServerTerrainApproach_Implementation(const FVector_NetQuantize100& HitPoint, float Height, float Strength, int Radius);

	void InputCameraZoom(float AxisValue);

	void StartDrag();
	void EndDrag();

	int EditCategory = -1;
	int EditMode = -1;

	float TargetHeight = 0.0;

	ADynamicSplineSection* EditSplineSection = nullptr;
	USplineComponent* EditSpline = nullptr;
	UDynamicSplinePoint* EditSplinePoint = nullptr;
	USphereComponent* EditSplineDummy = nullptr;
	//FVector EditSplineStart;

	//FVector LastHit;
	bool GetMouseHit(FHitResult& OutHit, ECollisionChannel channel = ECC_Visibility, const TArray<UPrimitiveComponent*>& Ignore = TArray<UPrimitiveComponent*>());

	void StartMouse();
	void EndMouse();
public:
	//UFUNCTION(Server, Unreliable)
	//void ServerMovementUpdate(FVector Velocity);
	//void ServerMovementUpdate_Implementation(FVector Velocity);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if UE_BUILD_DEBUG
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(Category = "WorldPawn|Events", BlueprintCallable)
	void SetEditCategory(int category);
	UFUNCTION(Category = "WorldPawn|Events", BlueprintCallable)
	void SetEditMode(int mode);
	UFUNCTION(Category = "WorldPawn|Events", BlueprintCallable)
	void SetTargetHeight(float height);

	//FVector Velocity;
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
