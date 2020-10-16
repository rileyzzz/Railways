// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTypes.h"

//bool FProcSkeletalMeshVertex::GetRigidWeightBone(FBoneIndexType& OutBoneIndex) const
//{
//	bool bIsRigid = false;
//
//	for (int32 WeightIdx = 0; WeightIdx < MAX_TOTAL_INFLUENCES; WeightIdx++)
//	{
//		if (InfluenceWeights[WeightIdx] == 255)
//		{
//			bIsRigid = true;
//			OutBoneIndex = InfluenceBones[WeightIdx];
//			break;
//		}
//	}
//
//	return bIsRigid;
//}
//
//uint8 FProcSkeletalMeshVertex::GetMaximumWeight() const
//{
//	uint8 MaxInfluenceWeight = 0;
//
//	for (int32 Index = 0; Index < MAX_TOTAL_INFLUENCES; Index++)
//	{
//		const uint8 Weight = InfluenceWeights[Index];
//
//		if (Weight > MaxInfluenceWeight)
//		{
//			MaxInfluenceWeight = Weight;
//		}
//	}
//
//	return MaxInfluenceWeight;
//}