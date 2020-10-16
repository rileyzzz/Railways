// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralSkinWeightVertexBuffer.h"
#include "EngineUtils.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "RenderUtils.h"
#include "SkeletalMeshTypes.h"
#include "UObject/AnimObjectVersion.h"
#include "ProfilingDebugging/LoadTimeTracker.h"
//
///*-----------------------------------------------------------------------------
//FProcSkinWeightLookupVertexBuffer
//-----------------------------------------------------------------------------*/
//
//FProcSkinWeightLookupVertexBuffer::FProcSkinWeightLookupVertexBuffer()
//	: bNeedsCPUAccess(false)
//	, LookupData(nullptr)
//	, Data(nullptr)
//	, NumVertices(0)
//{
//}
//
//FProcSkinWeightLookupVertexBuffer::FProcSkinWeightLookupVertexBuffer(const FProcSkinWeightLookupVertexBuffer& Other)
//	: bNeedsCPUAccess(Other.bNeedsCPUAccess)
//	, LookupData(nullptr)
//	, Data(nullptr)
//	, NumVertices(0)
//{
//
//}
//
//FProcSkinWeightLookupVertexBuffer::~FProcSkinWeightLookupVertexBuffer()
//{
//	CleanUp();
//}
//
//FProcSkinWeightLookupVertexBuffer& FProcSkinWeightLookupVertexBuffer::operator=(const FProcSkinWeightLookupVertexBuffer& Other)
//{
//	CleanUp();
//	bNeedsCPUAccess = Other.bNeedsCPUAccess;
//	return *this;
//}
//
//void FProcSkinWeightLookupVertexBuffer::CleanUp()
//{
//	if (LookupData)
//	{
//		delete LookupData;
//		LookupData = NULL;
//	}
//}
//
//bool FProcSkinWeightLookupVertexBuffer::IsLookupDataValid() const
//{
//	return LookupData != NULL;
//}
//
//void FProcSkinWeightLookupVertexBuffer::Init(uint32 InNumVertices)
//{
//	AllocateData();
//	ResizeBuffer(InNumVertices);
//}
//
//void FProcSkinWeightLookupVertexBuffer::ResizeBuffer(uint32 InNumVertices)
//{
//	NumVertices = InNumVertices;
//	LookupData->ResizeBuffer(NumVertices);
//	Data = NumVertices ? LookupData->GetDataPointer() : nullptr;
//}
//
//FArchive& operator<<(FArchive& Ar, FProcSkinWeightLookupVertexBuffer& VertexBuffer)
//{
//	FStripDataFlags StripFlags(Ar);
//
//	Ar.UsingCustomVersion(FAnimObjectVersion::GUID);
//	VertexBuffer.SerializeMetaData(Ar);
//
//	if (Ar.IsLoading() || VertexBuffer.LookupData == NULL)
//	{
//		// If we're loading, or we have no valid buffer, allocate container.
//		VertexBuffer.AllocateData();
//	}
//
//	// if Ar is counting, it still should serialize. Need to count VertexData
//	if (!StripFlags.IsDataStrippedForServer() || Ar.IsCountingMemory())
//	{
//		if (VertexBuffer.LookupData != NULL)
//		{
//			if (Ar.CustomVer(FAnimObjectVersion::GUID) >= FAnimObjectVersion::UnlimitedBoneInfluences)
//			{
//				VertexBuffer.LookupData->Serialize(Ar);
//			}
//
//			if (!Ar.IsCountingMemory())
//			{
//				// update cached buffer info
//				VertexBuffer.Data = (VertexBuffer.NumVertices > 0 && VertexBuffer.LookupData->GetResourceArray()->GetResourceDataSize()) ? VertexBuffer.LookupData->GetDataPointer() : nullptr;
//			}
//		}
//	}
//
//	return Ar;
//}
//
//void FProcSkinWeightLookupVertexBuffer::SerializeMetaData(FArchive& Ar)
//{
//	Ar.UsingCustomVersion(FAnimObjectVersion::GUID);
//	if (Ar.CustomVer(FAnimObjectVersion::GUID) >= FAnimObjectVersion::UnlimitedBoneInfluences)
//	{
//		Ar << NumVertices;
//	}
//}
//
//void FProcSkinWeightLookupVertexBuffer::CopyMetaData(const FProcSkinWeightLookupVertexBuffer& Other)
//{
//	NumVertices = Other.NumVertices;
//}
//
//void FProcSkinWeightLookupVertexBuffer::AllocateData()
//{
//	// Clear any old WeightData before allocating.
//	CleanUp();
//
//	LookupData = new TStaticMeshVertexData<uint32>(bNeedsCPUAccess);
//}
//
//void FProcSkinWeightLookupVertexBuffer::InitRHI()
//{
//	// BUF_ShaderResource is needed for support of the SkinCache (we could make is dependent on GEnableGPUSkinCacheShaders or are there other users?)
//	VertexBufferRHI = CreateRHIBuffer_RenderThread();
//
//	if (VertexBufferRHI)
//	{
//		bool bSRV = GSupportsResourceView && GPixelFormats[PixelFormat].Supported;
//		// When bAllowCPUAccess is true, the meshes is likely going to be used for Niagara to spawn particles on mesh surface.
//		// And it can be the case for CPU *and* GPU access: no differenciation today. That is why we create a SRV in this case.
//		// This also avoid setting lots of states on all the members of all the different buffers used by meshes. Follow up: https://jira.it.epicgames.net/browse/UE-69376.
//		bSRV |= GetNeedsCPUAccess();
//
//		if (bSRV)
//		{
//			SRVValue = RHICreateShaderResourceView(FShaderResourceViewInitializer(LookupData ? VertexBufferRHI : nullptr, PixelFormat));
//		}
//	}
//}
//
//void FProcSkinWeightLookupVertexBuffer::ReleaseRHI()
//{
//	SRVValue.SafeRelease();
//
//	FVertexBuffer::ReleaseRHI();
//}
//
//FVertexBufferRHIRef FProcSkinWeightLookupVertexBuffer::CreateRHIBuffer_RenderThread()
//{
//	return CreateRHIBuffer_Internal<true>();
//}
//
//FVertexBufferRHIRef FProcSkinWeightLookupVertexBuffer::CreateRHIBuffer_Async()
//{
//	return CreateRHIBuffer_Internal<false>();
//}
//
//void FProcSkinWeightLookupVertexBuffer::GetWeightOffsetAndInfluenceCount(uint32 VertexIndex, uint32& OutWeightOffset, uint32& OutInfluenceCount) const
//{
//	uint32 Offset = VertexIndex * 4;
//	uint32 DataUInt32 = *((uint32*)(&Data[Offset]));
//	OutWeightOffset = DataUInt32 >> 8;
//	OutInfluenceCount = DataUInt32 & 0xff;
//}
//
//void FProcSkinWeightLookupVertexBuffer::SetWeightOffsetAndInfluenceCount(uint32 VertexIndex, uint32 WeightOffset, uint32 InfluenceCount)
//{
//	uint32 Offset = VertexIndex * 4;
//	uint32* DataUInt32 = (uint32*)(&Data[Offset]);
//	*DataUInt32 = (WeightOffset << 8) | InfluenceCount;
//}
//
//template <bool bRenderThread>
//FVertexBufferRHIRef FProcSkinWeightLookupVertexBuffer::CreateRHIBuffer_Internal()
//{
//	if (NumVertices)
//	{
//		// Create the vertex buffer.
//		FResourceArrayInterface* ResourceArray = LookupData ? LookupData->GetResourceArray() : nullptr;
//		const uint32 SizeInBytes = ResourceArray ? ResourceArray->GetResourceDataSize() : 0;
//		const uint32 BuffFlags = BUF_Static | BUF_ShaderResource;
//		FRHIResourceCreateInfo CreateInfo(ResourceArray);
//		CreateInfo.bWithoutNativeResource = !LookupData;
//
//		if (bRenderThread)
//		{
//			return RHICreateVertexBuffer(SizeInBytes, BuffFlags, CreateInfo);
//		}
//		else
//		{
//			return RHIAsyncCreateVertexBuffer(SizeInBytes, BuffFlags, CreateInfo);
//		}
//	}
//	return nullptr;
//}
//
///*-----------------------------------------------------------------------------
//FProcSkinWeightDataVertexBuffer
//-----------------------------------------------------------------------------*/
//
//FProcSkinWeightDataVertexBuffer::FProcSkinWeightDataVertexBuffer()
//	: bNeedsCPUAccess(false)
//	, bVariableBonesPerVertex(false)
//	, MaxBoneInfluences(MAX_INFLUENCES_PER_STREAM)
//	, bUse16BitBoneIndex(false)
//	, WeightData(nullptr)
//	, Data(nullptr)
//	, NumVertices(0)
//	, NumBones(0)
//{
//}
//
//FProcSkinWeightDataVertexBuffer::FProcSkinWeightDataVertexBuffer(const FProcSkinWeightDataVertexBuffer& Other)
//	: bNeedsCPUAccess(Other.bNeedsCPUAccess)
//	, bVariableBonesPerVertex(Other.bVariableBonesPerVertex)
//	, MaxBoneInfluences(Other.MaxBoneInfluences)
//	, bUse16BitBoneIndex(Other.bUse16BitBoneIndex)
//	, WeightData(nullptr)
//	, Data(nullptr)
//	, NumVertices(0)
//	, NumBones(0)
//{
//
//}
//
//FProcSkinWeightDataVertexBuffer::~FProcSkinWeightDataVertexBuffer()
//{
//	CleanUp();
//}
//
//FProcSkinWeightDataVertexBuffer& FProcSkinWeightDataVertexBuffer::operator=(const FProcSkinWeightDataVertexBuffer& Other)
//{
//	CleanUp();
//	bNeedsCPUAccess = Other.bNeedsCPUAccess;
//	bVariableBonesPerVertex = Other.bVariableBonesPerVertex;
//	MaxBoneInfluences = Other.MaxBoneInfluences;
//	bUse16BitBoneIndex = Other.bUse16BitBoneIndex;
//	return *this;
//}
//
//void FProcSkinWeightDataVertexBuffer::CleanUp()
//{
//	if (WeightData)
//	{
//		delete WeightData;
//		WeightData = NULL;
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::Init(uint32 InNumBones, uint32 InNumVertices)
//{
//	AllocateData();
//	ResizeBuffer(InNumBones, InNumVertices);
//}
//
//void FProcSkinWeightDataVertexBuffer::ResizeBuffer(uint32 InNumBones, uint32 InNumVertices)
//{
//	NumBones = InNumBones;
//	NumVertices = InNumVertices;
//	WeightData->ResizeBuffer(GetVertexDataSize());
//
//	if (NumBones > 0)
//	{
//		Data = WeightData->GetDataPointer();
//	}
//}
//
//FArchive& operator<<(FArchive& Ar, FProcSkinWeightDataVertexBuffer& VertexBuffer)
//{
//	FStripDataFlags StripFlags(Ar);
//
//	Ar.UsingCustomVersion(FAnimObjectVersion::GUID);
//	VertexBuffer.SerializeMetaData(Ar);
//
//	if (Ar.IsLoading() || VertexBuffer.WeightData == NULL)
//	{
//		// If we're loading, or we have no valid buffer, allocate container.
//		VertexBuffer.AllocateData();
//	}
//
//	// if Ar is counting, it still should serialize. Need to count VertexData
//	if (!StripFlags.IsDataStrippedForServer() || Ar.IsCountingMemory())
//	{
//		if (VertexBuffer.WeightData != NULL)
//		{
//			if (Ar.IsLoading() && Ar.CustomVer(FAnimObjectVersion::GUID) < FAnimObjectVersion::UnlimitedBoneInfluences)
//			{
//				FStaticMeshVertexDataInterface* LegacyWeightData = nullptr;
//				bool bExtraBoneInfluences = VertexBuffer.MaxBoneInfluences > MAX_INFLUENCES_PER_STREAM;
//				if (bExtraBoneInfluences)
//				{
//					if (VertexBuffer.bUse16BitBoneIndex)
//					{
//						LegacyWeightData = new FSkinWeightVertexData< TLegacySkinWeightInfo<true, FBoneIndex16> >(VertexBuffer.bNeedsCPUAccess);
//					}
//					else
//					{
//						LegacyWeightData = new FSkinWeightVertexData< TLegacySkinWeightInfo<true, FBoneIndex8> >(VertexBuffer.bNeedsCPUAccess);
//					}
//				}
//				else
//				{
//					if (VertexBuffer.bUse16BitBoneIndex)
//					{
//						LegacyWeightData = new FSkinWeightVertexData< TLegacySkinWeightInfo<false, FBoneIndex16> >(VertexBuffer.bNeedsCPUAccess);
//					}
//					else
//					{
//						LegacyWeightData = new FSkinWeightVertexData< TLegacySkinWeightInfo<false, FBoneIndex8> >(VertexBuffer.bNeedsCPUAccess);
//					}
//				}
//				bool bForcePerElementSerialization = Ar.IsLoading() && Ar.CustomVer(FAnimObjectVersion::GUID) < FAnimObjectVersion::IncreaseBoneIndexLimitPerChunk;
//				LegacyWeightData->Serialize(Ar, bForcePerElementSerialization);
//
//				uint32 BufferSize = LegacyWeightData->GetResourceArray()->GetResourceDataSize();
//				check(VertexBuffer.GetVertexDataSize() == BufferSize);
//				VertexBuffer.WeightData->ResizeBuffer(BufferSize);
//				FMemory::Memcpy(VertexBuffer.WeightData->GetDataPointer(), LegacyWeightData->GetDataPointer(), BufferSize);
//				VertexBuffer.NumVertices = LegacyWeightData->Num();
//
//				if (LegacyWeightData)
//				{
//					delete LegacyWeightData;
//				}
//			}
//			else
//			{
//				VertexBuffer.WeightData->Serialize(Ar);
//			}
//
//			if (!Ar.IsCountingMemory())
//			{
//				// update cached buffer info
//				VertexBuffer.Data = (VertexBuffer.NumBones > 0 && VertexBuffer.WeightData->GetResourceArray()->GetResourceDataSize()) ? VertexBuffer.WeightData->GetDataPointer() : nullptr;
//			}
//		}
//	}
//
//	return Ar;
//}
//
//void FProcSkinWeightDataVertexBuffer::SerializeMetaData(FArchive& Ar)
//{
//	Ar.UsingCustomVersion(FSkeletalMeshCustomVersion::GUID);
//	Ar.UsingCustomVersion(FAnimObjectVersion::GUID);
//
//	if (Ar.IsLoading() && Ar.CustomVer(FAnimObjectVersion::GUID) < FAnimObjectVersion::UnlimitedBoneInfluences)
//	{
//		bool bExtraBoneInfluences = false;
//		if (Ar.CustomVer(FSkeletalMeshCustomVersion::GUID) < FSkeletalMeshCustomVersion::SplitModelAndRenderData)
//		{
//			Ar << bExtraBoneInfluences << NumVertices;
//		}
//		else
//		{
//			uint32 Stride = 0;
//			Ar << bExtraBoneInfluences << Stride << NumVertices;
//		}
//		MaxBoneInfluences = bExtraBoneInfluences ? EXTRA_BONE_INFLUENCES : MAX_INFLUENCES_PER_STREAM;
//		NumBones = MaxBoneInfluences * NumVertices;
//		bVariableBonesPerVertex = false;
//	}
//	else
//	{
//		Ar << bVariableBonesPerVertex << MaxBoneInfluences << NumBones << NumVertices;
//	}
//
//	// bUse16BitBoneIndex doesn't exist before version IncreaseBoneIndexLimitPerChunk
//	if (Ar.CustomVer(FAnimObjectVersion::GUID) >= FAnimObjectVersion::IncreaseBoneIndexLimitPerChunk)
//	{
//		Ar << bUse16BitBoneIndex;
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::CopyMetaData(const FProcSkinWeightDataVertexBuffer& Other)
//{
//	bVariableBonesPerVertex = Other.bVariableBonesPerVertex;
//	MaxBoneInfluences = Other.MaxBoneInfluences;
//	bUse16BitBoneIndex = Other.bUse16BitBoneIndex;
//	NumBones = Other.NumBones;
//}
//
//template <bool bRenderThread>
//FVertexBufferRHIRef FProcSkinWeightDataVertexBuffer::CreateRHIBuffer_Internal()
//{
//	if (NumBones)
//	{
//		// Create the vertex buffer.
//		FResourceArrayInterface* ResourceArray = WeightData ? WeightData->GetResourceArray() : nullptr;
//		const uint32 SizeInBytes = ResourceArray ? ResourceArray->GetResourceDataSize() : 0;
//		const uint32 BuffFlags = BUF_Static | BUF_ShaderResource;
//		FRHIResourceCreateInfo CreateInfo(ResourceArray);
//		CreateInfo.bWithoutNativeResource = !WeightData;
//
//		// BUF_ShaderResource is needed for support of the SkinCache (we could make is dependent on GEnableGPUSkinCacheShaders or are there other users?)
//		if (bRenderThread)
//		{
//			return RHICreateVertexBuffer(SizeInBytes, BuffFlags, CreateInfo);
//		}
//		else
//		{
//			return RHIAsyncCreateVertexBuffer(SizeInBytes, BuffFlags, CreateInfo);
//		}
//	}
//	return nullptr;
//}
//
//FVertexBufferRHIRef FProcSkinWeightDataVertexBuffer::CreateRHIBuffer_RenderThread()
//{
//	return CreateRHIBuffer_Internal<true>();
//}
//
//FVertexBufferRHIRef FProcSkinWeightDataVertexBuffer::CreateRHIBuffer_Async()
//{
//	return CreateRHIBuffer_Internal<false>();
//}
//
//void FProcSkinWeightDataVertexBuffer::InitRHI()
//{
//	SCOPED_LOADTIMER(FProcSkinWeightVertexBuffer_InitRHI);
//
//	// BUF_ShaderResource is needed for support of the SkinCache (we could make is dependent on GEnableGPUSkinCacheShaders or are there other users?)
//	VertexBufferRHI = CreateRHIBuffer_RenderThread();
//
//	bool bSRV = VertexBufferRHI && GSupportsResourceView && GPixelFormats[GetPixelFormat()].Supported;
//	// When bAllowCPUAccess is true, the meshes is likely going to be used for Niagara to spawn particles on mesh surface.
//	// And it can be the case for CPU *and* GPU access: no differenciation today. That is why we create a SRV in this case.
//	// This also avoid setting lots of states on all the members of all the different buffers used by meshes. Follow up: https://jira.it.epicgames.net/browse/UE-69376.
//	bSRV |= GetNeedsCPUAccess();
//
//	if (bSRV)
//	{
//		SRVValue = RHICreateShaderResourceView(FShaderResourceViewInitializer(WeightData ? VertexBufferRHI : nullptr, GetPixelFormat()));
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::ReleaseRHI()
//{
//	SRVValue.SafeRelease();
//
//	FVertexBuffer::ReleaseRHI();
//}
//
//void FProcSkinWeightDataVertexBuffer::AllocateData()
//{
//	// Clear any old WeightData before allocating.
//	CleanUp();
//
//	WeightData = new FSkinWeightVertexData<uint8>(bNeedsCPUAccess);
//}
//
//void FProcSkinWeightDataVertexBuffer::SetMaxBoneInfluences(uint32 InMaxBoneInfluences)
//{
//	check(InMaxBoneInfluences <= MAX_TOTAL_INFLUENCES);
//	if (InMaxBoneInfluences <= MAX_INFLUENCES_PER_STREAM)
//		MaxBoneInfluences = MAX_INFLUENCES_PER_STREAM;
//	else if (InMaxBoneInfluences <= EXTRA_BONE_INFLUENCES)
//		MaxBoneInfluences = EXTRA_BONE_INFLUENCES;
//	else
//		MaxBoneInfluences = MAX_TOTAL_INFLUENCES;
//
//	bVariableBonesPerVertex = FGPUBaseSkinVertexFactory::UseUnlimitedBoneInfluences(MaxBoneInfluences);
//	if (MaxBoneInfluences == MAX_TOTAL_INFLUENCES && !bVariableBonesPerVertex)
//	{
//		MaxBoneInfluences = EXTRA_BONE_INFLUENCES;
//		UE_LOG(LogSkeletalMesh, Error, TEXT("Skeletal mesh of %d bone influences requires unlimited bone influence mode. Influence truncation will occur and will not render correctly."), InMaxBoneInfluences);
//	}
//}
//
//bool FProcSkinWeightDataVertexBuffer::GetRigidWeightBone(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, int32& OutBoneIndex) const
//{
//	bool bIsRigid = false;
//
//	uint8* BoneData = Data + VertexWeightOffset;
//	uint32 BoneWeightOffset = GetBoneIndexByteSize() * VertexInfluenceCount;
//	for (uint32 InfluenceIndex = 0; InfluenceIndex < VertexInfluenceCount; InfluenceIndex++)
//	{
//		uint8 BoneWeight = BoneData[BoneWeightOffset + InfluenceIndex];
//		if (BoneWeight == 255)
//		{
//			bIsRigid = true;
//			if (Use16BitBoneIndex())
//			{
//				FBoneIndex16* BoneIndex16Ptr = (FBoneIndex16*)BoneData;
//				OutBoneIndex = BoneIndex16Ptr[InfluenceIndex];
//			}
//			else
//			{
//				OutBoneIndex = BoneData[InfluenceIndex];
//			}
//			break;
//		}
//	}
//
//	return bIsRigid;
//}
//
//uint32 FProcSkinWeightDataVertexBuffer::GetBoneIndex(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex) const
//{
//	if (InfluenceIndex < VertexInfluenceCount)
//	{
//		uint8* BoneData = Data + VertexWeightOffset;
//		if (Use16BitBoneIndex())
//		{
//			FBoneIndex16* BoneIndex16Ptr = (FBoneIndex16*)BoneData;
//			return BoneIndex16Ptr[InfluenceIndex];
//		}
//		else
//		{
//			return BoneData[InfluenceIndex];
//		}
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::SetBoneIndex(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex, uint32 BoneIndex)
//{
//	if (InfluenceIndex < VertexInfluenceCount)
//	{
//		uint8* BoneData = Data + VertexWeightOffset;
//		if (Use16BitBoneIndex())
//		{
//			FBoneIndex16* BoneIndex16Ptr = (FBoneIndex16*)BoneData;
//			BoneIndex16Ptr[InfluenceIndex] = BoneIndex;
//		}
//		else
//		{
//			BoneData[InfluenceIndex] = BoneIndex;
//		}
//	}
//	else
//	{
//		check(false);
//	}
//}
//
//uint8 FProcSkinWeightDataVertexBuffer::GetBoneWeight(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex) const
//{
//	if (InfluenceIndex < VertexInfluenceCount)
//	{
//		uint8* BoneData = Data + VertexWeightOffset;
//		uint32 BoneWeightOffset = GetBoneIndexByteSize() * VertexInfluenceCount;
//		return BoneData[BoneWeightOffset + InfluenceIndex];
//	}
//	else
//	{
//		return 0;
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::SetBoneWeight(uint32 VertexWeightOffset, uint32 VertexInfluenceCount, uint32 InfluenceIndex, uint8 BoneWeight)
//{
//	if (InfluenceIndex < VertexInfluenceCount)
//	{
//		uint8* BoneData = Data + VertexWeightOffset;
//		uint32 BoneWeightOffset = GetBoneIndexByteSize() * VertexInfluenceCount;
//		BoneData[BoneWeightOffset + InfluenceIndex] = BoneWeight;
//	}
//	else
//	{
//		check(false);
//	}
//}
//
//void FProcSkinWeightDataVertexBuffer::ResetVertexBoneWeights(uint32 VertexWeightOffset, uint32 VertexInfluenceCount)
//{
//	if (VertexInfluenceCount > 0)
//	{
//		uint8* BoneData = Data + VertexWeightOffset;
//		if (BoneData)
//		{
//			if (Use16BitBoneIndex())
//			{
//				FMemory::Memzero(BoneData, (sizeof(FBoneIndex16) + sizeof(uint8)) * VertexInfluenceCount);
//			}
//			else
//			{
//				FMemory::Memzero(BoneData, (sizeof(FBoneIndex8) + sizeof(uint8)) * VertexInfluenceCount);
//			}
//		}
//	}
//}
//
//
///*-----------------------------------------------------------------------------
//FProcSkinWeightVertexBuffer
//-----------------------------------------------------------------------------*/
//
//FProcSkinWeightVertexBuffer::FProcSkinWeightVertexBuffer()
//{
//}
//
//FProcSkinWeightVertexBuffer::FProcSkinWeightVertexBuffer(const FProcSkinWeightVertexBuffer& Other)
//{
//	bool bNeedsCPUAccess = Other.GetNeedsCPUAccess();
//	DataVertexBuffer.SetNeedsCPUAccess(bNeedsCPUAccess);
//	LookupVertexBuffer.SetNeedsCPUAccess(bNeedsCPUAccess);
//
//	SetMaxBoneInfluences(Other.GetMaxBoneInfluences());
//	SetUse16BitBoneIndex(Other.Use16BitBoneIndex());
//}
//
//FProcSkinWeightVertexBuffer::~FProcSkinWeightVertexBuffer()
//{
//	CleanUp();
//}
//
//FProcSkinWeightVertexBuffer& FProcSkinWeightVertexBuffer::operator=(const FProcSkinWeightVertexBuffer& Other)
//{
//	CleanUp();
//
//	bool bNeedsCPUAccess = Other.GetNeedsCPUAccess();
//	DataVertexBuffer.SetNeedsCPUAccess(bNeedsCPUAccess);
//	LookupVertexBuffer.SetNeedsCPUAccess(bNeedsCPUAccess);
//
//	SetMaxBoneInfluences(Other.GetMaxBoneInfluences());
//	SetUse16BitBoneIndex(Other.Use16BitBoneIndex());
//
//	return *this;
//}
//
//void FProcSkinWeightVertexBuffer::CleanUp()
//{
//	DataVertexBuffer.CleanUp();
//	LookupVertexBuffer.CleanUp();
//}
//
//FArchive& operator<<(FArchive& Ar, FProcSkinWeightVertexBuffer& VertexBuffer)
//{
//	Ar.UsingCustomVersion(FAnimObjectVersion::GUID);
//	Ar << VertexBuffer.DataVertexBuffer;
//
//	if (Ar.IsLoading() && Ar.CustomVer(FAnimObjectVersion::GUID) < FAnimObjectVersion::UnlimitedBoneInfluences)
//	{
//		// LookupVertexBuffer doesn't exist before this version, so construct its content from scratch
//		uint32 MaxBoneInfluences = VertexBuffer.DataVertexBuffer.GetMaxBoneInfluences();
//		uint32 NumVertices = VertexBuffer.DataVertexBuffer.GetNumBones() / VertexBuffer.DataVertexBuffer.GetMaxBoneInfluences();
//		VertexBuffer.LookupVertexBuffer.Init(NumVertices);
//
//		uint32 WeightOffset = 0;
//		for (uint32 VertIdx = 0; VertIdx < NumVertices; VertIdx++)
//		{
//			VertexBuffer.LookupVertexBuffer.SetWeightOffsetAndInfluenceCount(VertIdx, WeightOffset * (VertexBuffer.GetBoneIndexByteSize() + sizeof(uint8)), MaxBoneInfluences);
//			WeightOffset += MaxBoneInfluences;
//		}
//	}
//	else
//	{
//		Ar << VertexBuffer.LookupVertexBuffer;
//	}
//
//	return Ar;
//}
//
//void FProcSkinWeightVertexBuffer::SerializeMetaData(FArchive& Ar)
//{
//	DataVertexBuffer.SerializeMetaData(Ar);
//	LookupVertexBuffer.SerializeMetaData(Ar);
//}
//
//void FProcSkinWeightVertexBuffer::CopyMetaData(const FProcSkinWeightVertexBuffer& Other)
//{
//	DataVertexBuffer.CopyMetaData(Other.DataVertexBuffer);
//	LookupVertexBuffer.CopyMetaData(Other.LookupVertexBuffer);
//}
//
//FSkinWeightRHIInfo FProcSkinWeightVertexBuffer::CreateRHIBuffer_RenderThread()
//{
//	FSkinWeightRHIInfo RHIInfo;
//	RHIInfo.DataVertexBufferRHI = DataVertexBuffer.CreateRHIBuffer_RenderThread();
//	RHIInfo.LookupVertexBufferRHI = LookupVertexBuffer.CreateRHIBuffer_RenderThread();
//	return RHIInfo;
//}
//
//FSkinWeightRHIInfo FProcSkinWeightVertexBuffer::CreateRHIBuffer_Async()
//{
//	FSkinWeightRHIInfo RHIInfo;
//	RHIInfo.DataVertexBufferRHI = DataVertexBuffer.CreateRHIBuffer_Async();
//	RHIInfo.LookupVertexBufferRHI = LookupVertexBuffer.CreateRHIBuffer_Async();
//	return RHIInfo;
//}
//
//GPUSkinBoneInfluenceType FProcSkinWeightVertexBuffer::GetBoneInfluenceType() const
//{
//	if (GetVariableBonesPerVertex())
//	{
//		return GPUSkinBoneInfluenceType::UnlimitedBoneInfluence;
//	}
//	else if (GetMaxBoneInfluences() > MAX_INFLUENCES_PER_STREAM)
//	{
//		return GPUSkinBoneInfluenceType::ExtraBoneInfluence;
//	}
//	else
//	{
//		return GPUSkinBoneInfluenceType::DefaultBoneInfluence;
//	}
//}
//
//void FProcSkinWeightVertexBuffer::GetVertexInfluenceOffsetCount(uint32 VertexIndex, uint32& VertexWeightOffset, uint32& VertexInfluenceCount) const
//{
//	if (GetVariableBonesPerVertex())
//	{
//		LookupVertexBuffer.GetWeightOffsetAndInfluenceCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	}
//	else
//	{
//		VertexWeightOffset = GetConstantInfluencesVertexStride() * VertexIndex;
//		VertexInfluenceCount = GetMaxBoneInfluences();
//	}
//}
//
//bool FProcSkinWeightVertexBuffer::GetRigidWeightBone(uint32 VertexIndex, int32& OutBoneIndex) const
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	return DataVertexBuffer.GetRigidWeightBone(VertexWeightOffset, VertexInfluenceCount, OutBoneIndex);
//}
//
//uint32 FProcSkinWeightVertexBuffer::GetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex) const
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	return DataVertexBuffer.GetBoneIndex(VertexWeightOffset, VertexInfluenceCount, InfluenceIndex);
//}
//
//void FProcSkinWeightVertexBuffer::SetBoneIndex(uint32 VertexIndex, uint32 InfluenceIndex, uint32 BoneIndex)
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	DataVertexBuffer.SetBoneIndex(VertexWeightOffset, VertexInfluenceCount, InfluenceIndex, BoneIndex);
//}
//
//uint8 FProcSkinWeightVertexBuffer::GetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex) const
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	return DataVertexBuffer.GetBoneWeight(VertexWeightOffset, VertexInfluenceCount, InfluenceIndex);
//}
//
//void FProcSkinWeightVertexBuffer::SetBoneWeight(uint32 VertexIndex, uint32 InfluenceIndex, uint8 BoneWeight)
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	DataVertexBuffer.SetBoneWeight(VertexWeightOffset, VertexInfluenceCount, InfluenceIndex, BoneWeight);
//}
//
//void FProcSkinWeightVertexBuffer::ResetVertexBoneWeights(uint32 VertexIndex)
//{
//	uint32 VertexWeightOffset = 0;
//	uint32 VertexInfluenceCount = 0;
//	GetVertexInfluenceOffsetCount(VertexIndex, VertexWeightOffset, VertexInfluenceCount);
//	DataVertexBuffer.ResetVertexBoneWeights(VertexWeightOffset, VertexInfluenceCount);
//}
//
//void FProcSkinWeightVertexBuffer::BeginInitResources()
//{
//	BeginInitResource(&LookupVertexBuffer);
//	BeginInitResource(&DataVertexBuffer);
//}
//
//void FProcSkinWeightVertexBuffer::BeginReleaseResources()
//{
//	BeginReleaseResource(&LookupVertexBuffer);
//	BeginReleaseResource(&DataVertexBuffer);
//}
//
//void FProcSkinWeightVertexBuffer::ReleaseResources()
//{
//	check(IsInRenderingThread());
//	LookupVertexBuffer.ReleaseResource();
//	DataVertexBuffer.ReleaseResource();
//}
//
//void FProcSkinWeightVertexBuffer::InitDynamic(const TArray<FProcSkeletalMeshVertex>& InVertices)
//{
//	TArray<FSkinWeightInfo> VertexWeightInfos;
//	VertexWeightInfos.AddUninitialized(InVertices.Num());
//
//	for (int32 VertIdx = 0; VertIdx < InVertices.Num(); VertIdx++)
//	{
//		const FProcSkeletalMeshVertex& SrcVertex = InVertices[VertIdx];
//		FSkinWeightInfo& DstVertex = VertexWeightInfos[VertIdx];
//		FMemory::Memcpy(DstVertex.InfluenceBones, SrcVertex.InfluenceBones, MAX_TOTAL_INFLUENCES * sizeof(FBoneIndexType));
//		FMemory::Memcpy(DstVertex.InfluenceWeights, SrcVertex.InfluenceWeights, MAX_TOTAL_INFLUENCES * sizeof(uint8));
//
//	}
//
//	*this = VertexWeightInfos;
//}
//
//FProcSkinWeightVertexBuffer& FProcSkinWeightVertexBuffer::operator=(const TArray<FSkinWeightInfo>& InVertices)
//{
//	uint32 TotalNumUsedBones = 0;
//	bool bVariableBonesPerVertex = GetVariableBonesPerVertex();
//
//	if (bVariableBonesPerVertex)
//	{
//		LookupVertexBuffer.Init(InVertices.Num());
//		for (int32 VertIdx = 0; VertIdx < InVertices.Num(); VertIdx++)
//		{
//			int32 NumUsedBones = 0;
//			for (int32 InfluenceIdx = 0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++)
//			{
//				if (InVertices[VertIdx].InfluenceWeights[InfluenceIdx] > 0)
//				{
//					NumUsedBones++;
//				}
//			}
//
//			LookupVertexBuffer.SetWeightOffsetAndInfluenceCount(VertIdx, TotalNumUsedBones * (GetBoneIndexByteSize() + sizeof(uint8)), NumUsedBones);
//			TotalNumUsedBones += NumUsedBones;
//		}
//	}
//	else
//	{
//		TotalNumUsedBones = GetMaxBoneInfluences() * InVertices.Num();
//	}
//
//	DataVertexBuffer.Init(TotalNumUsedBones, InVertices.Num());
//	for (int32 VertIdx = 0; VertIdx < InVertices.Num(); VertIdx++)
//	{
//		if (bVariableBonesPerVertex)
//		{
//			// Copy the weights and sort in descending order
//			FSkinWeightInfo VertexWeights = InVertices[VertIdx];
//			for (uint32 i = 0; i < MAX_TOTAL_INFLUENCES; i++)
//			{
//				uint32 MaxWeightIdx = i;
//				for (uint32 j = i + 1; j < MAX_TOTAL_INFLUENCES; j++)
//				{
//					if (VertexWeights.InfluenceWeights[j] > VertexWeights.InfluenceWeights[MaxWeightIdx])
//					{
//						MaxWeightIdx = j;
//					}
//				}
//				if (MaxWeightIdx != i)
//				{
//					Exchange(VertexWeights.InfluenceBones[i], VertexWeights.InfluenceBones[MaxWeightIdx]);
//					Exchange(VertexWeights.InfluenceWeights[i], VertexWeights.InfluenceWeights[MaxWeightIdx]);
//				}
//			}
//
//			int32 NumUsedBones = 0;
//			for (uint32 InfluenceIdx = 0; InfluenceIdx < GetMaxBoneInfluences(); InfluenceIdx++)
//			{
//				if (VertexWeights.InfluenceWeights[InfluenceIdx] > 0)
//				{
//					SetBoneIndex(VertIdx, NumUsedBones, VertexWeights.InfluenceBones[InfluenceIdx]);
//					SetBoneWeight(VertIdx, NumUsedBones, VertexWeights.InfluenceWeights[InfluenceIdx]);
//					NumUsedBones++;
//				}
//			}
//		}
//		else
//		{
//			const FSkinWeightInfo& VertexWeights = InVertices[VertIdx];
//			for (uint32 InfluenceIdx = 0; InfluenceIdx < GetMaxBoneInfluences(); InfluenceIdx++)
//			{
//				SetBoneIndex(VertIdx, InfluenceIdx, VertexWeights.InfluenceBones[InfluenceIdx]);
//				SetBoneWeight(VertIdx, InfluenceIdx, VertexWeights.InfluenceWeights[InfluenceIdx]);
//			}
//		}
//	}
//
//	return *this;
//}
//
//void FProcSkinWeightVertexBuffer::GetSkinWeights(TArray<FSkinWeightInfo>& OutVertices) const
//{
//	OutVertices.SetNum(GetNumVertices());
//	for (uint32 VertIdx = 0; VertIdx < GetNumVertices(); VertIdx++)
//	{
//		for (int32 InfluenceIdx = 0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++)
//		{
//			OutVertices[VertIdx].InfluenceBones[InfluenceIdx] = GetBoneIndex(VertIdx, InfluenceIdx);
//			OutVertices[VertIdx].InfluenceWeights[InfluenceIdx] = GetBoneWeight(VertIdx, InfluenceIdx);
//		}
//	}
//}
//
//FSkinWeightInfo FProcSkinWeightVertexBuffer::GetVertexSkinWeights(uint32 VertexIndex) const
//{
//	FSkinWeightInfo OutVertex;
//	for (int32 InfluenceIdx = 0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++)
//	{
//		OutVertex.InfluenceBones[InfluenceIdx] = GetBoneIndex(VertexIndex, InfluenceIdx);
//		OutVertex.InfluenceWeights[InfluenceIdx] = GetBoneWeight(VertexIndex, InfluenceIdx);
//	}
//	return OutVertex;
//}