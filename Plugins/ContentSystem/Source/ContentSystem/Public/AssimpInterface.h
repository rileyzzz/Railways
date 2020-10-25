// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MeshContent.h"
//#include "ProceduralSkeletalMeshComponent.h"
#include "AssimpInterface.generated.h"



UCLASS()
class CONTENTSYSTEM_API UAssimpInterface : public UObject
{
	GENERATED_BODY()
private:
	//void ProcessNode(const FString& FilePath, const aiScene* scene, aiNode* node, AssimpNode& Parent);

public:
	RailwaysImportData* ImportFBX(bool PretransformVerts);
	//void BuildComponent(UProceduralSkeletalMeshComponent* Component, AssimpImportData* Data);
};
