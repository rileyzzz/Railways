// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalMeshCreator.h"

SkeletalMeshCreator::SkeletalMeshCreator()
{
}

//void SkeletalMeshCreator::BuildMeshInternal(const FReferenceSkeleton& RefSkeleton, const TArray<SkeletalMeshImportData::FVertInfluence>& Influences, const TArray<SkeletalMeshImportData::FMeshWedge>& Wedges, const TArray<SkeletalMeshImportData::FMeshFace>& Faces, const TArray<FVector>& Points, const TArray<int32>& PointToOriginalMap)
//{
//
//}
//
//
//void SkeletalMeshCreator::BuildLOD(USkeletalMesh* SkeletalMesh, FSkeletalMeshImportData& ImportData, const int32 LODLevel)
//{
//	TArray<FVector> LODPoints;
//	TArray<SkeletalMeshImportData::FMeshWedge> LODWedges;
//	TArray<SkeletalMeshImportData::FMeshFace> LODFaces;
//	TArray<SkeletalMeshImportData::FVertInfluence> LODInfluences;
//	TArray<int32> LODPointToRawMap;
//	ImportData.CopyLODImportData(LODPoints, LODWedges, LODFaces, LODInfluences, LODPointToRawMap);
//
//	int32 TexCoordCount = FMath::Max<int32>(1, ImportData.NumTexCoords);
//
//	SkeletalMesh = NewObject<USkeletalMesh>(this, USkeletalMesh::StaticClass(), NAME_None);
//	//MeshUtilities.BuildSkeletalMesh(
//	//	BuildLODModel,
//	//	RefSkeleton,
//	//	LODInfluences,
//	//	LODWedges,
//	//	LODFaces,
//	//	LODPoints,
//	//	LODPointToRawMap,
//	//	Options
//	//);
//
//	//meshutilities source
//	//SkeletalMeshBuildData BuildData(
//	//	LODModel,
//	//	RefSkeleton,
//	//	Influences,
//	//	Wedges,
//	//	Faces,
//	//	Points,
//	//	PointToOriginalMap,
//	//	BuildOptions,
//	//	OutWarningMessages,
//	//	OutWarningNames);
//
//	//FSkeletalMeshUtilityBuilder Builder;
//	//if (!Builder.PrepareSourceMesh(&BuildData))
//	//{
//	//	return false;
//	//}
//
//	//if (!Builder.GenerateSkeletalRenderMesh(&BuildData))
//	//{
//	//	return false;
//	//}
//
//	//then
//	//SkeletalMeshTools::BuildSkeletalMeshChunks(BuildData.Faces, RawVertices, VertIndexAndZ, BuildData.BuildOptions.OverlappingThresholds, BuildData.Chunks, BuildData.bTooManyVerts);
//	//then
//	//SkeletalMeshTools::ChunkSkinnedVertices(BuildData.Chunks, AlternateBoneIDs, MaxGPUSkinBones);
//
//
//	//BuildSkeletalModelFromChunks(BuildData.LODModel, BuildData.RefSkeleton, BuildData.Chunks, BuildData.PointToOriginalMap);
//}

SkeletalMeshCreator::~SkeletalMeshCreator()
{
}
