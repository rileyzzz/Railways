// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AssimpInterface.h"
//#include "ContentSystemInterface.h"
#include "ContentSystem/ContentSystemInterface.h"
#include "RailwaysGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RAILWAYS_API URailwaysGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	UContentSystemInterface* Interface;
public:
	UAssimpInterface* AssimpInterface;

	virtual void Init() override;
	virtual void Shutdown() override;
};
