// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralSkeletalMeshComponent.h"
#include "AssimpInterface.generated.h"

struct CONTENTSYSTEM_API AssimpVert
{
	FVector Location;
	FVector Normal;
	FVector Tangent;
	FVector2D TexCoords;

	AssimpVert(FVector InLocation, FVector InNormal, FVector InTangent, FVector2D InTexCoords) : Location(InLocation), Normal(InNormal), Tangent(InTangent), TexCoords(InTexCoords) { }
};

struct CONTENTSYSTEM_API AssimpMesh
{
	FString Name;
	TArray<AssimpVert> Vertices;
	TArray<uint32> Elements;
};

struct CONTENTSYSTEM_API AssimpImportData
{
	TArray<AssimpMesh*> Meshes;
};

UCLASS()
class CONTENTSYSTEM_API UAssimpInterface : public UObject
{
	GENERATED_BODY()
public:
	AssimpImportData* ImportFBX();
	void BuildComponent(UProceduralSkeletalMeshComponent* Component, AssimpImportData* Data);
};
