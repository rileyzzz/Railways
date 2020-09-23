// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AssimpInterface.generated.h"

struct CONTENTSYSTEM_API AssimpImportData
{

};

UCLASS()
class CONTENTSYSTEM_API UAssimpInterface : public UObject
{
	GENERATED_BODY()
public:
	AssimpImportData* ImportFBX();
};
