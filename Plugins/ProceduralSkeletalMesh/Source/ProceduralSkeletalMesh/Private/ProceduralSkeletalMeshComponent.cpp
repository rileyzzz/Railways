// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralSkeletalMeshComponent.h"
#include "SkeletalMeshTypes.h"
#include "Rendering/SkeletalMeshRenderData.h"


//feel really bad about some of this stuff being so close to ProceduralMeshComponent, but come on this tangent stuff is nuts

//class FProcSkeletalMeshProxySection
//{
//public:
//	/** Material applied to this section */
//	UMaterialInterface* Material;
//	/** Vertex buffers for this section */
//	FStaticMeshVertexBuffers VertexBuffers;
//	//need my own skin weight buffer class that works at runtime
//	FProcSkinWeightVertexBuffer SkinWeightVertexBuffer;
//
//	/** Index buffer for this section */
//	FDynamicMeshIndexBuffer32 IndexBuffer;
//	/** Vertex factory for this section */
//	FLocalVertexFactory VertexFactory;
//	/** Whether this section is currently visible */
//	bool bSectionVisible;
//
//	TArray<FBoneIndexType> ActiveBoneIndices;
//
//	TArray<FBoneIndexType> RequiredBones;
//
//	FProcSkeletalMeshProxySection(ERHIFeatureLevel::Type InFeatureLevel)
//		: Material(NULL)
//		, VertexFactory(InFeatureLevel, "FProcSkeletalMeshProxySection")
//		, bSectionVisible(true)
//	{}
//};
//
//class FProcSkeletalMeshSectionUpdateData
//{
//public:
//	/** Section to update */
//	int32 TargetSection;
//	/** New vertex information */
//	TArray<FProcSkeletalMeshVertex> NewVertexBuffer;
//};
//
//static void ConvertProcMeshToDynMeshVertex(FDynamicMeshVertex& Vert, const FProcSkeletalMeshVertex& ProcVert)
//{
//	Vert.Position = ProcVert.Position;
//	Vert.TextureCoordinate[0] = ProcVert.UV0;
//	Vert.TangentX = ProcVert.Tangent.TangentX;
//	Vert.TangentZ = ProcVert.Normal;
//	Vert.TangentZ.Vector.W = ProcVert.Tangent.bFlipTangentY ? -127 : 127;
//}
//
//class FProceduralSkeletalMeshSceneProxy final : public FPrimitiveSceneProxy
//{
//private:
//	TArray<FProcSkeletalMeshProxySection*> Sections;
//	FMaterialRelevance MaterialRelevance;
//
//	friend class UProceduralSkeletalMeshComponent;
//	friend class FSkinWeightVertexBuffer;
//public:
//	SIZE_T GetTypeHash() const override
//	{
//		static size_t UniquePointer;
//		return reinterpret_cast<size_t>(&UniquePointer);
//	}
//
//	//TArray<SkeletalMeshData> LoadedMeshes;
//	//void InitBuffersWithSize(uint32 NumVertices, uint32 NumIndices)
//	//{
//	//	VertexBuffers.InitWithDummyData(&VertexFactory, NumVertices);
//	//	IndexBuffer.NumIndices = NumIndices;
//	//	BeginInitResource(&IndexBuffer);
//	//}
//
//
//	void ProcessNode(UProceduralSkeletalMeshComponent* Component, FProcSkeletalMeshNode* Node)
//	{
//		// Copy each section to a proxy
//		const int32 NumSections = Node->Sections.Num();
//		Sections.AddZeroed(NumSections);
//		for (int SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("creating proxy section"));
//			FProcSkeletalMeshSection& SrcSection = Node->Sections[SectionIdx];
//			if (SrcSection.ProcIndexBuffer.Num() > 0 && SrcSection.ProcVertexBuffer.Num() > 0)
//			{
//				FProcSkeletalMeshProxySection* NewSection = new FProcSkeletalMeshProxySection(GetScene().GetFeatureLevel());
//
//				// Copy data from vertex buffer
//				const int32 NumVerts = SrcSection.ProcVertexBuffer.Num();
//
//				// Allocate verts
//				TArray<FDynamicMeshVertex> Vertices;
//				Vertices.SetNumUninitialized(NumVerts);
//				// Copy verts
//				for (int VertIdx = 0; VertIdx < NumVerts; VertIdx++)
//				{
//					const FProcSkeletalMeshVertex& ProcVert = SrcSection.ProcVertexBuffer[VertIdx];
//					FDynamicMeshVertex& Vert = Vertices[VertIdx];
//					ConvertProcMeshToDynMeshVertex(Vert, ProcVert);
//				}
//
//				// Copy index buffer
//				NewSection->IndexBuffer.Indices = SrcSection.ProcIndexBuffer;
//
//				NewSection->VertexBuffers.InitFromDynamicVertex(&NewSection->VertexFactory, Vertices, 4);
//
//				//init skin weights
//				NewSection->SkinWeightVertexBuffer.SetNeedsCPUAccess(true);
//				NewSection->SkinWeightVertexBuffer.SetMaxBoneInfluences(4);
//				NewSection->SkinWeightVertexBuffer.SetUse16BitBoneIndex(false);
//				NewSection->SkinWeightVertexBuffer.InitDynamic(SrcSection.ProcVertexBuffer); //add weight info
//
//
//				/*NewSection->SkinWeightVertexBuffer.SetNeedsCPUAccess(true);
//				NewSection->SkinWeightVertexBuffer.SetMaxBoneInfluences(4);
//				NewSection->SkinWeightVertexBuffer.SetUse16BitBoneIndex(false);
//				NewSection->SkinWeightVertexBuffer.Init(Vertices);*/
//
//				// Enqueue initialization of render resource
//				BeginInitResource(&NewSection->VertexBuffers.PositionVertexBuffer);
//				BeginInitResource(&NewSection->VertexBuffers.StaticMeshVertexBuffer);
//				NewSection->SkinWeightVertexBuffer.BeginInitResources();
//				//BeginInitResource(&NewSection->VertexBuffers.ColorVertexBuffer);
//				BeginInitResource(&NewSection->IndexBuffer);
//				BeginInitResource(&NewSection->VertexFactory);
//
//				// Grab material
//				NewSection->Material = Component->GetMaterial(SectionIdx);
//				if (NewSection->Material == NULL)
//				{
//					NewSection->Material = UMaterial::GetDefaultMaterial(MD_Surface);
//				}
//
//				// Copy visibility info
//				NewSection->bSectionVisible = SrcSection.bSectionVisible;
//
//				// Save ref to new section
//				Sections[SectionIdx] = NewSection;
//			}
//		}
//
//		for (auto& Child : Node->ChildNodes)
//			ProcessNode(Component, &Child);
//	}
//
//	FProceduralSkeletalMeshSceneProxy(UProceduralSkeletalMeshComponent* Component, FProcSkeletalMeshNode* RootNode)
//		: FPrimitiveSceneProxy(Component)
//		//: FSkeletalMeshSceneProxy(Component, InSkelMeshRenderData)
//		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Entering scene proxy constructor"));
//		ProcessNode(Component, RootNode);
//	}
//
//	virtual ~FProceduralSkeletalMeshSceneProxy() //not sure why this is virtual lol, the class is final
//	{
//		for (FProcSkeletalMeshProxySection* Section : Sections)
//		{
//			if (Section != nullptr)
//			{
//				Section->VertexBuffers.PositionVertexBuffer.ReleaseResource();
//				Section->VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
//				Section->VertexBuffers.ColorVertexBuffer.ReleaseResource();
//				Section->IndexBuffer.ReleaseResource();
//				Section->VertexFactory.ReleaseResource();
//
//				delete Section;
//			}
//		}
//	}
//
//	/** Called on render thread to assign new dynamic data */
//	void UpdateSection_RenderThread(FProcSkeletalMeshSectionUpdateData* SectionData)
//	{
//		check(IsInRenderingThread());
//
//		// Check we have data
//		if (SectionData != nullptr)
//		{
//			// Check it references a valid section
//			if (SectionData->TargetSection < Sections.Num() &&
//				Sections[SectionData->TargetSection] != nullptr)
//			{
//				FProcSkeletalMeshProxySection* Section = Sections[SectionData->TargetSection];
//
//				// Lock vertex buffer
//				const int32 NumVerts = SectionData->NewVertexBuffer.Num();
//
//				// Iterate through vertex data, copying in new info
//				for (int32 i = 0; i < NumVerts; i++)
//				{
//					const FProcSkeletalMeshVertex& ProcVert = SectionData->NewVertexBuffer[i];
//					FDynamicMeshVertex Vertex;
//					ConvertProcMeshToDynMeshVertex(Vertex, ProcVert);
//
//					Section->VertexBuffers.PositionVertexBuffer.VertexPosition(i) = Vertex.Position;
//					Section->VertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(i, Vertex.TangentX.ToFVector(), Vertex.GetTangentY(), Vertex.TangentZ.ToFVector());
//					Section->VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(i, 0, Vertex.TextureCoordinate[0]);
//					//Section->VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(i, 1, Vertex.TextureCoordinate[1]);
//					//Section->VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(i, 2, Vertex.TextureCoordinate[2]);
//					//Section->VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(i, 3, Vertex.TextureCoordinate[3]);
//					//Section->VertexBuffers.ColorVertexBuffer.VertexColor(i) = Vertex.Color;
//					for (int32 b = 0; b < MAX_TOTAL_INFLUENCES; b++)
//					{
//						Section->SkinWeightVertexBuffer.SetBoneIndex(i, b, ProcVert.InfluenceBones[b]);
//						Section->SkinWeightVertexBuffer.SetBoneWeight(i, b, ProcVert.InfluenceWeights[b]);
//					}
//					
//				}
//
//				//position
//				{
//					auto& VertexBuffer = Section->VertexBuffers.PositionVertexBuffer;
//					void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetNumVertices() * VertexBuffer.GetStride(), RLM_WriteOnly);
//					FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetNumVertices() * VertexBuffer.GetStride());
//					RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
//				}
//
//				//bone data
//				{
//					auto& VertexBuffer = Section->SkinWeightVertexBuffer.DataVertexBuffer;
//					void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetVertexDataSize(), RLM_WriteOnly);
//					FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetVertexDataSize());
//					RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
//				}
//
//				//bone lookup data
//				{
//					auto& VertexBuffer = Section->SkinWeightVertexBuffer.LookupVertexBuffer;
//					void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetNumVertices() * VertexBuffer.GetStride(), RLM_WriteOnly);
//					FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetNumVertices() * VertexBuffer.GetStride());
//					RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
//				}
//
//				//color
//				//{
//				//	auto& VertexBuffer = Section->VertexBuffers.ColorVertexBuffer;
//				//	void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetNumVertices() * VertexBuffer.GetStride(), RLM_WriteOnly);
//				//	FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetNumVertices() * VertexBuffer.GetStride());
//				//	RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
//				//}
//
//				//tangents
//				{
//					auto& VertexBuffer = Section->VertexBuffers.StaticMeshVertexBuffer;
//					void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTangentSize(), RLM_WriteOnly);
//					FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTangentData(), VertexBuffer.GetTangentSize());
//					RHIUnlockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI);
//				}
//
//				//texcoords
//				{
//					auto& VertexBuffer = Section->VertexBuffers.StaticMeshVertexBuffer;
//					void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
//					FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTexCoordData(), VertexBuffer.GetTexCoordSize());
//					RHIUnlockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);
//				}
//			}
//
//			// Free data sent from game thread
//			delete SectionData;
//		}
//	}
//
//	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
//	{
//		//SCOPE_CYCLE_COUNTER(STAT_ProcMesh_GetMeshElements);
//
//
//		// Set up wireframe material (if needed)
//		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
//
//		FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
//		if (bWireframe)
//		{
//			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
//				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
//				FLinearColor(0, 0.5f, 1.f)
//			);
//
//			Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
//		}
//
//		// Iterate over sections
//		for (const FProcSkeletalMeshProxySection* Section : Sections)
//		{
//			if (Section != nullptr && Section->bSectionVisible)
//			{
//				FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : Section->Material->GetRenderProxy();
//
//				// For each view..
//				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
//				{
//					if (VisibilityMap & (1 << ViewIndex))
//					{
//						const FSceneView* View = Views[ViewIndex];
//						// Draw the mesh.
//						FMeshBatch& Mesh = Collector.AllocateMesh();
//						FMeshBatchElement& BatchElement = Mesh.Elements[0];
//						BatchElement.IndexBuffer = &Section->IndexBuffer;
//						Mesh.bWireframe = bWireframe;
//						Mesh.VertexFactory = &Section->VertexFactory;
//						Mesh.MaterialRenderProxy = MaterialProxy;
//
//						bool bHasPrecomputedVolumetricLightmap;
//						FMatrix PreviousLocalToWorld;
//						int32 SingleCaptureIndex;
//						bool bOutputVelocity;
//						GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);
//
//						FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
//						DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, DrawsVelocity(), bOutputVelocity);
//						BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
//
//						BatchElement.FirstIndex = 0;
//						BatchElement.NumPrimitives = Section->IndexBuffer.Indices.Num() / 3;
//						BatchElement.MinVertexIndex = 0;
//						BatchElement.MaxVertexIndex = Section->VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
//						Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
//						Mesh.Type = PT_TriangleList;
//						Mesh.DepthPriorityGroup = SDPG_World;
//						Mesh.bCanApplyViewModeOverrides = false;
//						Collector.AddMesh(ViewIndex, Mesh);
//					}
//				}
//			}
//		}
//	}
//
//	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const
//	{
//		FPrimitiveViewRelevance Result;
//		Result.bDrawRelevance = IsShown(View);
//		Result.bShadowRelevance = IsShadowCast(View);
//		Result.bDynamicRelevance = true;
//		Result.bRenderInMainPass = ShouldRenderInMainPass();
//		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
//		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
//		Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
//		MaterialRelevance.SetPrimitiveViewRelevance(Result);
//		Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;
//		return Result;
//	}
//
//	virtual bool CanBeOccluded() const override
//	{
//		return !MaterialRelevance.bDisableDepthTest;
//	}
//
//	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
//
//	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }
//};
//
////======================================================================================================================================================================
////COMPONENT STUFF
////======================================================================================================================================================================
//
//UProceduralSkeletalMeshComponent::UProceduralSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	//bUseComplexAsSimpleCollision = true;
//}
//
//void RecursiveGetBounds(FBox& LocalBox, const FProcSkeletalMeshNode& Node)
//{
//	for (const auto& Section : Node.Sections)
//	{
//		LocalBox += Section.SectionLocalBox;
//		UE_LOG(LogTemp, Warning, TEXT("Adding collision box of size %f"), Section.SectionLocalBox.GetSize().Size());
//	}
//		
//	for (const auto& Child : Node.ChildNodes)
//		RecursiveGetBounds(LocalBox, Child);
//}
//
//void UProceduralSkeletalMeshComponent::UpdateLocalBounds()
//{
//	//fix this
//	FBox LocalBox(ForceInit);
//
//	//for (const FProcSkeletalMeshSection& Section : RootNode.Sections)
//		//LocalBox += Section.SectionLocalBox;
//
//	//RecursiveGetBounds(LocalBox, RootNode);
//	LocalBox += FBox(FVector(-400.0f, -400.0f, -400.0f), FVector(400.0f, 400.0f, 400.0f));
//
//	LocalBounds = LocalBox.IsValid ? FBoxSphereBounds(LocalBox) : FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0);
//
//	// Update global bounds
//	UpdateBounds();
//	// Need to send to render thread
//	MarkRenderTransformDirty();
//}
//
//void UProceduralSkeletalMeshComponent::OnRegister()
//{
//	Super::OnRegister();
//
//}
//
//void UProceduralSkeletalMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//
//}
//
////void UProceduralSkeletalMeshComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
////{
////	Super::CreateRenderState_Concurrent(Context);
////
////	//SendRenderDynamicData_Concurrent();
////}
//
//FBoxSphereBounds UProceduralSkeletalMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
//{
//	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));
//
//	Ret.BoxExtent *= BoundsScale;
//	Ret.SphereRadius *= BoundsScale;
//
//	return Ret;
//}
//
//FPrimitiveSceneProxy* UProceduralSkeletalMeshComponent::CreateSceneProxy()
//{
////	LLM_SCOPE(ELLMTag::SkeletalMesh);
////	ERHIFeatureLevel::Type SceneFeatureLevel = GetWorld()->FeatureLevel;
////	FSkeletalMeshSceneProxy* Result = nullptr;
////	FSkeletalMeshRenderData* SkelMeshRenderData = GetSkeletalMeshRenderData();
////
////	// Only create a scene proxy for rendering if properly initialized
////	if (SkelMeshRenderData &&
////		SkelMeshRenderData->LODRenderData.IsValidIndex(PredictedLODLevel) &&
////		!bHideSkin &&
////		MeshObject)
////	{
////		// Only create a scene proxy if the bone count being used is supported, or if we don't have a skeleton (this is the case with destructibles)
////		int32 MaxBonesPerChunk = SkelMeshRenderData->GetMaxBonesPerSection();
////		int32 MaxSupportedNumBones = GetFeatureLevelMaxNumberOfBones(SceneFeatureLevel);
////		if (MaxBonesPerChunk <= MaxSupportedNumBones)
////		{
////			Result = ::new FSkeletalMeshSceneProxy(this, SkelMeshRenderData);
////		}
////	}
////
////#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
////	SendRenderDebugPhysics(Result);
////#endif
////
////	return Result;
//	return new FProceduralSkeletalMeshSceneProxy(this, &RootNode);
//}
//
//void UProceduralSkeletalMeshComponent::PostLoad()
//{
//	Super::PostLoad();
//
//	//if (ProcMeshBodySetup && IsTemplate())
//	//{
//	//	ProcMeshBodySetup->SetFlags(RF_Public);
//	//}
//}
//
//int32 RecursiveGetNumMaterials(const FProcSkeletalMeshNode& Node)
//{
//	int32 Count = Node.Sections.Num();
//	for (const auto& child : Node.ChildNodes)
//	{
//		Count += RecursiveGetNumMaterials(child);
//	}
//	return Count;
//}
//
//int32 UProceduralSkeletalMeshComponent::GetNumMaterials() const
//{
//	//return ProcSkeletalMeshSections.Num();
//	return RecursiveGetNumMaterials(RootNode);
//}
//
//void UProceduralSkeletalMeshComponent::CreateMeshSection(int32 SectionIndex, FProcSkeletalMeshNode& ParentNode, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents)
//{
//	UE_LOG(LogTemp, Warning, TEXT("creating mesh section"));
//	// Ensure sections array is long enough
//	if (SectionIndex >= ParentNode.Sections.Num())
//	{
//		ParentNode.Sections.SetNum(SectionIndex + 1, false);
//	}
//
//	// Reset this section (in case it already existed)
//	FProcSkeletalMeshSection& NewSection = ParentNode.Sections[SectionIndex];
//	NewSection.Reset();
//
//	// Copy data to vertex buffer
//	const int32 NumVerts = Vertices.Num();
//	NewSection.ProcVertexBuffer.Reset();
//	NewSection.ProcVertexBuffer.AddUninitialized(NumVerts);
//	for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
//	{
//		FProcSkeletalMeshVertex& Vertex = NewSection.ProcVertexBuffer[VertIdx];
//
//		Vertex.Position = Vertices[VertIdx];
//		Vertex.Normal = (Normals.Num() == NumVerts) ? Normals[VertIdx] : FVector(0.f, 0.f, 1.f);
//		Vertex.UV0 = (UV.Num() == NumVerts) ? UV[VertIdx] : FVector2D(0.f, 0.f);
//		Vertex.Tangent = (Tangents.Num() == NumVerts) ? Tangents[VertIdx] : FProcSkeletalMeshTangent();
//
//		// Update bounding box
//		NewSection.SectionLocalBox += Vertex.Position;
//	}
//
//	// Copy index buffer (clamping to vertex range)
//	int32 NumTriIndices = Triangles.Num();
//	NumTriIndices = (NumTriIndices / 3) * 3; // Ensure we have exact number of triangles (array is multiple of 3 long)
//
//	NewSection.ProcIndexBuffer.Reset();
//	NewSection.ProcIndexBuffer.AddUninitialized(NumTriIndices);
//	for (int32 IndexIdx = 0; IndexIdx < NumTriIndices; IndexIdx++)
//	{
//		NewSection.ProcIndexBuffer[IndexIdx] = FMath::Min(Triangles[IndexIdx], NumVerts - 1);
//	}
//
//	//NewSection.bEnableCollision = bCreateCollision;
//
//	UpdateLocalBounds(); // Update overall bounds
//	//UpdateCollision(); // Mark collision as dirty
//	MarkRenderStateDirty(); // New section requires recreating scene proxy
//}
//
////void UProceduralSkeletalMeshComponent::UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV, const TArray<FProcSkeletalMeshTangent>& Tangents)
////{
////	if (SectionIndex < ProcSkeletalMeshSections.Num())
////	{
////		FProcSkeletalMeshSection& Section = ProcSkeletalMeshSections[SectionIndex];
////		const int32 NumVerts = Vertices.Num();
////		const int32 PreviousNumVerts = Section.ProcVertexBuffer.Num();
////
////		// Update bounds, if we are getting new position data
////		if (NumVerts == PreviousNumVerts)
////		{
////			Section.SectionLocalBox = FBox(Vertices);
////
////			// Iterate through vertex data, copying in new info
////			for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
////			{
////				FProcSkeletalMeshVertex& ModifyVert = Section.ProcVertexBuffer[VertIdx];
////
////				// Position data
////				if (Vertices.Num() == NumVerts)
////				{
////					ModifyVert.Position = Vertices[VertIdx];
////				}
////
////				// Normal data
////				if (Normals.Num() == NumVerts)
////				{
////					ModifyVert.Normal = Normals[VertIdx];
////				}
////
////				// Tangent data
////				if (Tangents.Num() == NumVerts)
////				{
////					ModifyVert.Tangent = Tangents[VertIdx];
////				}
////
////				// UV data
////				if (UV.Num() == NumVerts)
////				{
////					ModifyVert.UV0 = UV[VertIdx];
////				}
////			}
////
////			// If we have collision enabled on this section, update that too
////			//if (Section.bEnableCollision)
////			//{
////			//	TArray<FVector> CollisionPositions;
////
////			//	// We have one collision mesh for all sections, so need to build array of _all_ positions
////			//	for (const auto& CollisionSection : ProcSkeletalMeshSections)
////			//	{
////			//		// If section has collision, copy it
////			//		if (CollisionSection.bEnableCollision)
////			//		{
////			//			for (int32 VertIdx = 0; VertIdx < CollisionSection.ProcVertexBuffer.Num(); VertIdx++)
////			//			{
////			//				CollisionPositions.Add(CollisionSection.ProcVertexBuffer[VertIdx].Position);
////			//			}
////			//		}
////			//	}
////
////			//	// Pass new positions to trimesh
////			//	BodyInstance.UpdateTriMeshVertices(CollisionPositions);
////			//}
////
////			// If we have a valid proxy and it is not pending recreation
////			if (SceneProxy && !IsRenderStateDirty())
////			{
////				// Create data to update section
////				FProcSkeletalMeshSectionUpdateData* SectionData = new FProcSkeletalMeshSectionUpdateData;
////				SectionData->TargetSection = SectionIndex;
////				SectionData->NewVertexBuffer = Section.ProcVertexBuffer;
////
////				// Enqueue command to send to render thread
////				FProceduralSkeletalMeshSceneProxy* ProcSkeletalMeshSceneProxy = (FProceduralSkeletalMeshSceneProxy*)SceneProxy;
////				ENQUEUE_RENDER_COMMAND(FProcSkeletalMeshSectionUpdate)
////					([ProcSkeletalMeshSceneProxy, SectionData](FRHICommandListImmediate& RHICmdList) { ProcSkeletalMeshSceneProxy->UpdateSection_RenderThread(SectionData); });
////			}
////
////			UpdateLocalBounds();		 // Update overall bounds
////			MarkRenderTransformDirty();  // Need to send new bounds to render thread
////		}
////	}
////}
