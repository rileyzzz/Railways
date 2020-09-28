// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WorldEditPlayerController.h"
#include "../../World/HeightWorld.h"
#include "../../World/WorldTileDynamic.h"
#include "../../Core/Objects/DynamicSplineSection.h"
#include "WorldEditorPawn.generated.h"

UCLASS()
class RAILWAYS_API AWorldEditorPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWorldEditorPawn();
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MainMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* PaintMaterial;

	UDecalComponent* Cursor;
	UDecalComponent* EditCursor;

	USpringArmComponent* SpringArm;
	UCameraComponent* Camera;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool b_draggingMouse = false;
	bool b_leftMouse = false;
	AWorldEditPlayerController* Controller;

	float ForwardVelocity = 0.0f;
	float RightVelocity = 0.0f;
	void UpdatePositionToGround(FVector& Position);
	void InputFlyForward(float AxisValue);
	void InputFlyRight(float AxisValue);

	void InputCameraX(float AxisValue);
	void InputCameraY(float AxisValue);
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
