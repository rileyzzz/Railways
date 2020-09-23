// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Rendering/SkeletalMeshLODImporterData.h"

/**
 * 
 */
class PROCEDURALSKELETALMESH_API SkeletalMeshCreator
{
	//friend class USkeletalMesh;
private:
	//void BuildMeshInternal(const FReferenceSkeleton& RefSkeleton, const TArray<SkeletalMeshImportData::FVertInfluence>& Influences, const TArray<SkeletalMeshImportData::FMeshWedge>& Wedges, const TArray<SkeletalMeshImportData::FMeshFace>& Faces, const TArray<FVector>& Points, const TArray<int32>& PointToOriginalMap);
public:
	SkeletalMeshCreator();

	//void BuildLOD(USkeletalMesh* SkeletalMesh, FSkeletalMeshImportData& ImportData, const int32 LODLevel);

	~SkeletalMeshCreator();
};
