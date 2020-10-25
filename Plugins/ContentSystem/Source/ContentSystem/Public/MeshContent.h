// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeshContent.generated.h"

#define AIVEC3_TO_FVEC(vector) FVector(vector->x, vector->y, vector->z)
#define AIVEC3REF_TO_FVEC(vector) FVector(vector.x, vector.y, vector.z)
#define AIQUATREF_TO_FQUAT(quat) FQuat(quat.x, quat.y, quat.z, quat.w)

struct aiScene;
struct aiNode;

struct CONTENTSYSTEM_API RailwaysVert
{
	FVector Location;
	FVector Normal;
	FVector Tangent;
	FVector2D TexCoords;

	TArray<uint8> InfluenceWeights;
	TArray<uint16> InfluenceBones;

	RailwaysVert(FVector InLocation, FVector InNormal, FVector InTangent, FVector2D InTexCoords) : Location(InLocation), Normal(InNormal), Tangent(InTangent), TexCoords(InTexCoords) { }
	RailwaysVert() { }
};

enum class CONTENTSYSTEM_API TextureType : uint32
{
	Diffuse = 0,
	Normal = 1,
	Parameter = 2
};

struct CONTENTSYSTEM_API RailwaysTexture
{
	TextureType Type;
	FString Path;
	RailwaysTexture(TextureType InType, FString InPath) : Type(InType), Path(InPath) { }
	RailwaysTexture() { }
};

struct CONTENTSYSTEM_API RailwaysMaterial
{
	TArray<RailwaysTexture> Textures;
};

struct CONTENTSYSTEM_API RailwaysMesh
{
	FString Name;
	TArray<RailwaysVert> Vertices;
	TArray<uint32> Elements;
	//RailwaysMaterial Material;
	int32 MaterialIndex;
};

struct CONTENTSYSTEM_API RailwaysAnimationTrack
{
	FString Name;
	TArray<FVector> PosFrames;
	TArray<FQuat> RotFrames;
	TArray<FVector> ScaleFrames;
};

struct CONTENTSYSTEM_API RailwaysBone
{
	FString Name;
	FName FName;
	FMatrix OffsetTransform; //inverse bind/inverse bind pose
	FTransform Transform;
	int32 ParentIndex = INDEX_NONE;
	TArray<RailwaysAnimationTrack> AnimTracks; //should only really be one
};

struct CONTENTSYSTEM_API RailwaysNode
{
	FString Name;
	FMatrix LocalTransformMatrix;
	FTransform Transform;
	TArray<RailwaysMesh> Meshes;
	TArray<RailwaysNode> Children;
	RailwaysNode() {}
	RailwaysNode(const FString& FilePath, TMap<FString, uint32>& m_BoneMapping, TArray<RailwaysBone>& Bones, const aiScene* scene, aiNode* node, const FTransform& ParentTransform); //, aiMatrix4x4 ParentTransform
	void RecursiveParentEmpty(int32 ParentIdx);
	void ProcessBoneHierarchy(TMap<FString, uint32>& m_BoneMapping, TArray<RailwaysBone>& Bones, const FMatrix& ParentTransform, int ParentIndex);

	bool Serialize(FArchive& Ar);
};

struct CONTENTSYSTEM_API RailwaysAnimMetadata
{
	double Duration;
};

struct CONTENTSYSTEM_API RailwaysImportData
{
	RailwaysNode RootNode;
	TMap<FString, uint32> m_BoneMapping;
	TArray<RailwaysMaterial> Materials;
	TArray<RailwaysBone> Bones;
	TArray<RailwaysAnimMetadata> AnimData;
	//AssimpImportData(AssimpNode* InRootNode) : RootNode(InRootNode) { };
	RailwaysImportData()
	{

	}
};

USTRUCT()
struct CONTENTSYSTEM_API FMeshContent
{
	GENERATED_BODY()
public:
	RailwaysImportData* MeshData;
	FString DataPath; //textures etc
private:
	bool Serialize(FArchive& Ar);
public:
	void SaveMesh(FString Path);
	void LoadMesh(FString Path);

	FMeshContent();
	~FMeshContent();
};
