// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralSkeletalMeshComponent.h"
#include "AssimpInterface.generated.h"



struct aiScene;
struct aiNode;

struct CONTENTSYSTEM_API AssimpVert
{
	FVector Location;
	FVector Normal;
	FVector Tangent;
	FVector2D TexCoords;

	AssimpVert(FVector InLocation, FVector InNormal, FVector InTangent, FVector2D InTexCoords) : Location(InLocation), Normal(InNormal), Tangent(InTangent), TexCoords(InTexCoords) { }
};

enum class CONTENTSYSTEM_API TextureType
{
	Diffuse,
	Normal,
	Parameter
};

struct CONTENTSYSTEM_API AssimpTexture
{
	TextureType Type;
	FString Path;
	AssimpTexture(TextureType InType, FString InPath) : Type(InType), Path(InPath)
	{

	}
};

struct CONTENTSYSTEM_API AssimpMaterial
{
	TArray<AssimpTexture> Textures;
};

struct CONTENTSYSTEM_API AssimpMesh
{
	FString Name;
	TArray<AssimpVert> Vertices;
	TArray<uint32> Elements;
	AssimpMaterial Material;
};

struct CONTENTSYSTEM_API AssimpNode
{
	FString Name;
	FMatrix Transform;
	TArray<AssimpMesh*> Meshes;
	TArray<AssimpNode> Children;
	AssimpNode(const FString& FilePath, const aiScene* scene, aiNode* node);
};

struct CONTENTSYSTEM_API AssimpImportData
{
	AssimpNode* RootNode;
	AssimpImportData(AssimpNode* InRootNode) : RootNode(InRootNode) { };
};

UCLASS()
class CONTENTSYSTEM_API UAssimpInterface : public UObject
{
	GENERATED_BODY()
private:
	//void ProcessNode(const FString& FilePath, const aiScene* scene, aiNode* node, AssimpNode& Parent);

public:
	AssimpImportData* ImportFBX();
	void BuildComponent(UProceduralSkeletalMeshComponent* Component, AssimpImportData* Data);
};
