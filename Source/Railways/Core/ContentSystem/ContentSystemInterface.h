// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContentSystemInterface.generated.h"

enum class ContentType
{
	Route = 0,
	Session = 1,
	Object = 2,
	Locomotive = 3,
	RollingStock = 4,
	Interior = 5,
	Bogey = 6
};

UCLASS()
class RAILWAYS_API UContentSystemInterface : public UObject
{
	GENERATED_BODY()
public:
	static UContentSystemInterface* GetInterface();
	static void ShutdownInterface();
private:
	FString ContentDir;

	//subdirs
	FString EditDir;
	FString ActiveDir;

	//type directories
	
public:
	UContentSystemInterface();
	~UContentSystemInterface();
};
