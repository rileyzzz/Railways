// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTypes.h"
#include "RenderResource.h"
#include "StaticMeshVertexData.h"
#include "GPUSkinPublicDefs.h"
#include "SkeletalMeshTypes.h"
#include "GPUSkinVertexFactory.h"
#include "UObject/AnimObjectVersion.h"

//struct FSoftSkinVertex
//{
//	FVector			Position;
//
//	// Tangent, U-direction
//	FVector			TangentX;
//	// Binormal, V-direction
//	FVector			TangentY;
//	// Normal
//	FVector4		TangentZ;
//
//	// UVs
//	FVector2D		UVs[MAX_TEXCOORDS];
//	// VertexColor
//	FColor			Color;
//	FBoneIndexType	InfluenceBones[MAX_TOTAL_INFLUENCES];
//	uint8			InfluenceWeights[MAX_TOTAL_INFLUENCES];
//
//	/** If this vert is rigidly weighted to a bone, return true and the bone index. Otherwise return false. */
//	ENGINE_API bool GetRigidWeightBone(FBoneIndexType& OutBoneIndex) const;
//
//	/** Returns the maximum weight of any bone that influences this vertex. */
//	ENGINE_API uint8 GetMaximumWeight() const;
//
//	/**
//	* Serializer
//	*
//	* @param Ar - archive to serialize with
//	* @param V - vertex to serialize
//	* @return archive that was used
//	*/
//	friend FArchive& operator<<(FArchive& Ar, FSoftSkinVertex& V);
//};

/** A vertex buffer storing bone index/weight data. */
//class FProcSkinWeightDataVertexBuffer : private FProcSkinWeightDataVertexBuffer
//{
//public:
//	/** Default constructor. */
//	FProcSkinWeightDataVertexBuffer();
//
//	/** Destructor. */
//	~FProcSkinWeightDataVertexBuffer();
//
//	//void Init(uint32 InNumBones, uint32 InNumVertices);
//};
/** A vertex buffer storing bone index/weight data. */
//class FProcSkinWeightDataVertexBuffer : public FVertexBuffer
//{
//public:
//	/** Default constructor. */
//	FProcSkinWeightDataVertexBuffer();
//
//	/** Constructor (copy) */
//	FProcSkinWeightDataVertexBuffer(const FProcSkinWeightDataVertexBuffer& Other);
//
//	/** Destructor. */
//	~FProcSkinWeightDataVertexBuffer();
//
//	/** Assignment. Assumes that vertex buffer will be rebuilt */
//	FProcSkinWeightDataVertexBuffer& operator=(const FProcSkinWeightDataVertexBuffer& Other);
//
//	/** Delete existing resources */
//	void CleanUp();
//
//	void Init(uint32 InNumBones, uint32 InNumVertices);
//
//	friend FArchive& operator<<(FArchive& Ar, FProcSkinWeightDataVertexBuffer& VertexBuffer);
//
//	void SerializeMetaData(FArchive& Ar);
//	void CopyMetaData(const FProcSkinWeightDataVertexBuffer& Other);
//
//	// FRenderResource interface.
//	virtual void InitRHI() override;
//	virtual void ReleaseRHI() override;
//
//	virtual FString GetFriendlyName() const override
//	{
//		return TEXT("SkeletalMesh Vertex Weights Data");
//	}
//
//	/** @return number of vertices in this vertex buffer */
//	FORCEINLINE uint32 GetNumVertices() const
//	{
//		return NumVertices;
//	}
//
//	void* GetVertexData()
//	{
//		return Data;
//	}
//
//	/** @return number of bones in this vertex buffer */
//	FORCEINLINE uint32 GetNumBones() const
//	{
//		return NumBones;
//	}
//
//	/** @return byte size of each bone index */
//	FORCEINLINE uint32 GetBoneIndexByteSize() const
//	{
//		return (Use16BitBoneIndex() ? sizeof(FBoneIndex16) : sizeof(FBoneIndex8));
//	}
//
//	/** @return vertex stride for when using constant number of bones per vertex buffer */
//	FORCEINLINE uint32 GetConstantInfluencesVertexStride() const
//	{
//		return (GetBoneIndexByteSize() + sizeof(uint8)) * MaxBoneInfluences;
//	}
//
//	/** @return offset position for bone weights data for each vertex */
//	FORCEINLINE uint32 GetConstantInfluencesBoneWeightsOffset() const
//	{
//		return GetBoneIndexByteSize() * MaxBoneInfluences;
//	}
//
//	/** @return total size of data in resource array */
//	FORCEINLINE uint32 GetVertexDataSize() const
//	{
//		return NumBones * (GetBoneIndexByteSize() + sizeof(uint8));
//	}
//
//	// @param guaranteed only to be valid if the vertex buffer is valid
//	FRHIShaderResourceView* GetSRV() const
//	{
//		return SRVValue;
//	}
//
//	/** Set if the CPU needs access to this vertex buffer */
//	FORCEINLINE void SetNeedsCPUAccess(bool bInNeedsCPUAccess)
//	{
//		bNeedsCPUAccess = bInNeedsCPUAccess;
//	}
//
//	FORCEINLINE bool GetNeedsCPUAccess() const
//	{
//		return bNeedsCPUAccess;
//	}
//
//	FORCEINLINE bool GetVariableBonesPerVertex() const
//	{
//		return bVariableBonesPerVertex;
//	}
//
//	/** Set if this will have extra streams for bone indices & weights. */
//	void SetMaxBoneInfluences(uint32 InMaxBoneInfluences);
//
//	FORCEINLINE uint32 GetMaxBoneInfluences() const
//	{
//		return MaxBoneInfluences;
//	}
//
//	FORCEINLINE void SetUse16BitBoneIndex(bool bInUse16BitBoneIndex)
//	{
//		bUse16BitBoneIndex = bInUse16BitBoneIndex;
//	}
//
//	FORCEINLINE bool Use16BitBoneIndex() const
//	{
//		return bUse16BitBoneIndex;
//	}
//
//	GPUSkinBoneInfluenceType GetBoneInfluenceType() const;
//	bool GetRigidWeightBone(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, int32& OutBoneIndex) const;
//	uint32 GetBoneIndex(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex) const;
//	void SetBoneIndex(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex, uint32 BoneIndex);
//	uint8 GetBoneWeight(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex) const;
//	void SetBoneWeight(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex, uint8 BoneWeight);
//	void ResetVertexBoneWeights(uint32 VertexWeightOffset, uint32 VertexInfluenceCount);
//
//	/** Create an RHI vertex buffer with CPU data. CPU data may be discarded after creation (see TResourceArray::Discard) */
//	FVertexBufferRHIRef CreateRHIBuffer_RenderThread();
//	FVertexBufferRHIRef CreateRHIBuffer_Async();
//
//	/** Similar to Init/ReleaseRHI but only update existing SRV so references to the SRV stays valid */
//	template <uint32 MaxNumUpdates>
//	void InitRHIForStreaming(FRHIVertexBuffer* IntermediateBuffer, TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		if (VertexBufferRHI && IntermediateBuffer)
//		{
//			check(SRVValue);
//			Batcher.QueueUpdateRequest(VertexBufferRHI, IntermediateBuffer);
//			Batcher.QueueUpdateRequest(SRVValue, VertexBufferRHI, GetPixelFormatStride(), GetPixelFormat());
//		}
//	}
//
//	template <uint32 MaxNumUpdates>
//	void ReleaseRHIForStreaming(TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		if (VertexBufferRHI)
//		{
//			Batcher.QueueUpdateRequest(VertexBufferRHI, nullptr);
//		}
//		if (SRVValue)
//		{
//			Batcher.QueueUpdateRequest(SRVValue, nullptr, 0, 0);
//		}
//	}
//
//protected:
//	// guaranteed only to be valid if the vertex buffer is valid
//	FShaderResourceViewRHIRef SRVValue;
//
//private:
//	void ResizeBuffer(uint32 InNumBones, uint32 InNumVertices);
//
//	EPixelFormat GetPixelFormat() const
//	{
//		return bVariableBonesPerVertex ? PF_R8_UINT : PF_R32_UINT;
//	}
//
//	uint32 GetPixelFormatStride() const
//	{
//		return bVariableBonesPerVertex ? 1 : 4;
//	}
//
//	/** true if this vertex buffer will be used with CPU skinning. Resource arrays are set to cpu accessible if this is true */
//	bool bNeedsCPUAccess;
//
//	bool bVariableBonesPerVertex;
//
//	/** Has extra bone influences per Vertex, which means using a different TGPUSkinVertexBase */
//	uint32 MaxBoneInfluences;
//
//	/** Use 16 bit bone index instead of 8 bit */
//	bool bUse16BitBoneIndex;
//
//	/** The vertex data storage type */
//	FStaticMeshVertexDataInterface* WeightData;
//
//	/** The cached vertex data pointer. */
//	uint8* Data;
//
//	/** The cached number of vertices. */
//	uint32 NumVertices;
//
//	/** The cached number of bones. */
//	uint32 NumBones;
//
//	/** Allocates the vertex data storage type. */
//	void AllocateData();
//
//	template <bool bRenderThread>
//	FVertexBufferRHIRef CreateRHIBuffer_Internal();
//};
//
//class FProcSkinWeightLookupVertexBuffer : public FVertexBuffer
//{
//public:
//	/** Default constructor. */
//	FProcSkinWeightLookupVertexBuffer();
//
//	/** Constructor (copy) */
//	FProcSkinWeightLookupVertexBuffer(const FProcSkinWeightLookupVertexBuffer& Other);
//
//	/** Destructor. */
//	~FProcSkinWeightLookupVertexBuffer();
//
//	/** Assignment. Assumes that vertex buffer will be rebuilt */
//	FProcSkinWeightLookupVertexBuffer& operator=(const FProcSkinWeightLookupVertexBuffer& Other);
//
//	/** Delete existing resources */
//	void CleanUp();
//
//	/** @return true is LookupData is valid */
//	bool IsLookupDataValid() const;
//
//	void Init(uint32 InNumVertices);
//
//	friend FArchive& operator<<(FArchive& Ar, FProcSkinWeightLookupVertexBuffer& VertexBuffer);
//
//	void SerializeMetaData(FArchive& Ar);
//	void CopyMetaData(const FProcSkinWeightLookupVertexBuffer& Other);
//
//	// FRenderResource interface.
//	virtual void InitRHI() override;
//	virtual void ReleaseRHI() override;
//
//	virtual FString GetFriendlyName() const override
//	{
//		return TEXT("SkeletalMesh Vertex Weights Lookup");
//	}
//
//	/** @return number of vertices in this vertex buffer */
//	FORCEINLINE uint32 GetNumVertices() const
//	{
//		return NumVertices;
//	}
//
//	void* GetVertexData()
//	{
//		return Data;
//	}
//
//	/** @return cached stride for vertex data type for this vertex buffer */
//	FORCEINLINE uint32 GetStride() const
//	{
//		return sizeof(uint32);
//	}
//
//	/** @return total size of data in resource array */
//	FORCEINLINE uint32 GetVertexDataSize() const
//	{
//		return NumVertices * GetStride();
//	}
//
//	// @param guaranteed only to be valid if the vertex buffer is valid
//	FRHIShaderResourceView* GetSRV() const
//	{
//		return SRVValue;
//	}
//
//	/** Set if the CPU needs access to this vertex buffer */
//	void SetNeedsCPUAccess(bool bInNeedsCPUAccess)
//	{
//		bNeedsCPUAccess = bInNeedsCPUAccess;
//	}
//
//	bool GetNeedsCPUAccess() const
//	{
//		return bNeedsCPUAccess;
//	}
//
//	/** Create an RHI vertex buffer with CPU data. CPU data may be discarded after creation (see TResourceArray::Discard) */
//	FVertexBufferRHIRef CreateRHIBuffer_RenderThread();
//	FVertexBufferRHIRef CreateRHIBuffer_Async();
//
//	/** Similar to Init/ReleaseRHI but only update existing SRV so references to the SRV stays valid */
//	template <uint32 MaxNumUpdates>
//	void InitRHIForStreaming(FRHIVertexBuffer* IntermediateBuffer, TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		if (VertexBufferRHI && IntermediateBuffer)
//		{
//			check(SRVValue);
//			Batcher.QueueUpdateRequest(VertexBufferRHI, IntermediateBuffer);
//			Batcher.QueueUpdateRequest(SRVValue, VertexBufferRHI, PixelFormatStride, PixelFormat);
//		}
//	}
//
//	template <uint32 MaxNumUpdates>
//	void ReleaseRHIForStreaming(TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		if (VertexBufferRHI)
//		{
//			Batcher.QueueUpdateRequest(VertexBufferRHI, nullptr);
//		}
//		if (SRVValue)
//		{
//			Batcher.QueueUpdateRequest(SRVValue, nullptr, 0, 0);
//		}
//	}
//
//	void GetWeightOffsetAndInfluenceCount(uint32 VertexIndex, uint32& OutWeightOffset, uint32& OutInfluenceCount) const;
//	void SetWeightOffsetAndInfluenceCount(uint32 VertexIndex, uint32 WeightOffset, uint32 InfluenceCount);
//
//protected:
//	// guaranteed only to be valid if the vertex buffer is valid
//	FShaderResourceViewRHIRef SRVValue;
//
//private:
//	void ResizeBuffer(uint32 InNumVertices);
//
//	/** Allocates the vertex data storage type. */
//	void AllocateData();
//
//	template <bool bRenderThread>
//	FVertexBufferRHIRef CreateRHIBuffer_Internal();
//
//	/** true if this vertex buffer will be used with CPU skinning. Resource arrays are set to cpu accessible if this is true */
//	bool bNeedsCPUAccess;
//
//	/** The vertex data storage type */
//	FStaticMeshVertexDataInterface* LookupData;
//
//	/** The cached vertex data pointer. */
//	uint8* Data;
//
//	/** The cached number of vertices. */
//	uint32 NumVertices;
//
//	static const EPixelFormat PixelFormat = PF_R32_UINT;
//	static const uint32 PixelFormatStride = 4;
//};
//
//class FProcSkinWeightVertexBuffer
//{
//public:
//	/** Default constructor. */
//	FProcSkinWeightVertexBuffer();
//
//	/** Constructor (copy) */
//	FProcSkinWeightVertexBuffer(const FProcSkinWeightVertexBuffer& Other);
//
//	/** Destructor. */
//	~FProcSkinWeightVertexBuffer();
//
//	/** Assignment. Assumes that vertex buffer will be rebuilt */
//	FProcSkinWeightVertexBuffer& operator=(const FProcSkinWeightVertexBuffer& Other);
//
//	/** Delete existing resources */
//	void CleanUp();
//
//	/** Init from another skin weight buffer */
//	void InitDynamic(const TArray<FProcSkeletalMeshVertex>& InVertices);
//
//	/** Assignment operator for assigning array of weights to this buffer */
//	FProcSkinWeightVertexBuffer& operator=(const TArray<FSkinWeightInfo>& InWeights);
//	void GetSkinWeights(TArray<FSkinWeightInfo>& OutVertices) const;
//	FSkinWeightInfo GetVertexSkinWeights(uint32 VertexIndex) const;
//
//	friend FArchive& operator<<(FArchive& Ar, FProcSkinWeightVertexBuffer& VertexBuffer);
//
//	void SerializeMetaData(FArchive& Ar);
//	void CopyMetaData(const FProcSkinWeightVertexBuffer& Other);
//
//	/** @return number of vertices in this vertex buffer */
//	FORCEINLINE uint32 GetNumVertices() const
//	{
//		return DataVertexBuffer.GetNumVertices();
//	}
//
//	/** @return total size of data in resource array */
//	FORCEINLINE uint32 GetVertexDataSize() const
//	{
//		return LookupVertexBuffer.GetVertexDataSize() + DataVertexBuffer.GetVertexDataSize();
//	}
//
//	void SetNeedsCPUAccess(bool bInNeedsCPUAccess)
//	{
//		DataVertexBuffer.SetNeedsCPUAccess(bInNeedsCPUAccess);
//		LookupVertexBuffer.SetNeedsCPUAccess(bInNeedsCPUAccess);
//	}
//
//	bool GetNeedsCPUAccess() const
//	{
//		return DataVertexBuffer.GetNeedsCPUAccess();
//	}
//
//	void SetMaxBoneInfluences(uint32 InMaxBoneInfluences)
//	{
//		DataVertexBuffer.SetMaxBoneInfluences(InMaxBoneInfluences);
//	}
//
//	uint32 GetMaxBoneInfluences() const
//	{
//		return DataVertexBuffer.GetMaxBoneInfluences();
//	}
//
//	void SetUse16BitBoneIndex(bool bInUse16BitBoneIndex)
//	{
//		DataVertexBuffer.SetUse16BitBoneIndex(bInUse16BitBoneIndex);
//	}
//
//	bool Use16BitBoneIndex() const
//	{
//		return DataVertexBuffer.Use16BitBoneIndex();
//	}
//
//	uint32 GetBoneIndexByteSize() const
//	{
//		return DataVertexBuffer.GetBoneIndexByteSize();
//	}
//
//	bool GetVariableBonesPerVertex() const
//	{
//		return DataVertexBuffer.GetVariableBonesPerVertex();
//	}
//
//	uint32 GetConstantInfluencesVertexStride() const
//	{
//		return DataVertexBuffer.GetConstantInfluencesVertexStride();
//	}
//
//	uint32 GetConstantInfluencesBoneWeightsOffset() const
//	{
//		return DataVertexBuffer.GetConstantInfluencesBoneWeightsOffset();
//	}
//
//	const FProcSkinWeightDataVertexBuffer* GetDataVertexBuffer() const
//	{
//		return &DataVertexBuffer;
//	}
//
//	const FProcSkinWeightLookupVertexBuffer* GetLookupVertexBuffer() const
//	{
//		return &LookupVertexBuffer;
//	}
//
//	FSkinWeightRHIInfo CreateRHIBuffer_RenderThread();
//	FSkinWeightRHIInfo CreateRHIBuffer_Async();
//
//	GPUSkinBoneInfluenceType GetBoneInfluenceType() const;
//	void GetVertexInfluenceOffsetCount(uint32 VertexIndex, uint32& VertexWeightOffset, uint32& VertexInfluenceCount) const;
//	bool GetRigidWeightBone(uint32 VertexIndex, int32& OutBoneIndex) const;
//	uint32 GetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex) const;
//	void SetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex, uint32 BoneIndex);
//	uint8 GetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex) const;
//	void SetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex, uint8 BoneWeight);
//	void ResetVertexBoneWeights(uint32 VertexIndex);
//
//	void BeginInitResources();
//	void BeginReleaseResources();
//	void ReleaseResources();
//
//	/** Similar to Init/ReleaseRHI but only update existing SRV so references to the SRV stays valid */
//	template <uint32 MaxNumUpdates>
//	void InitRHIForStreaming(const FSkinWeightRHIInfo& RHIInfo, TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		DataVertexBuffer.InitRHIForStreaming(RHIInfo.DataVertexBufferRHI, Batcher);
//		LookupVertexBuffer.InitRHIForStreaming(RHIInfo.DataVertexBufferRHI, Batcher);
//	}
//
//	template <uint32 MaxNumUpdates>
//	void ReleaseRHIForStreaming(TRHIResourceUpdateBatcher<MaxNumUpdates>& Batcher)
//	{
//		DataVertexBuffer.ReleaseRHIForStreaming(Batcher);
//		LookupVertexBuffer.ReleaseRHIForStreaming(Batcher);
//	}
//
////private:
//	/** Skin weights for skinning */
//	FProcSkinWeightDataVertexBuffer		DataVertexBuffer;
//
//	/** Skin weights lookup buffer */
//	FProcSkinWeightLookupVertexBuffer	LookupVertexBuffer;
//};