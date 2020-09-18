// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RuntimeMeshComponentStatic.h"
#include "WorldTile.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API UWorldTile : public URuntimeMeshComponentStatic
{
	GENERATED_BODY()

public:
	void Build(UMaterialInterface* Material);

};
