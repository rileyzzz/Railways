// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralSkeletalMeshComponent.h"

class FSkeletalIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), NumIndices * sizeof(int32), BUF_Dynamic, CreateInfo);
	}

	int32 NumIndices;
};

/** Dynamic data sent to render thread */
struct FSkeletonDynamicData
{
	/** Array of points */
	//TArray<FVector> CablePoints;
};

class FProceduralSkeletalMeshSceneProxy final : public FPrimitiveSceneProxy
{
private:

	UMaterialInterface* Material;
	FStaticMeshVertexBuffers VertexBuffers;
	FSkeletalIndexBuffer IndexBuffer;
	FLocalVertexFactory VertexFactory; //change this

	FSkeletonDynamicData* DynamicData;

	FMaterialRelevance MaterialRelevance;

	//int32 NumSegments;

	//float CableWidth;

	//int32 NumSides;

	//float TileMaterial;
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	TArray<SkeletalMeshData> LoadedMeshes;
	void InitBuffersWithSize(uint32 NumVertices, uint32 NumIndices)
	{
		VertexBuffers.InitWithDummyData(&VertexFactory, NumVertices);
		IndexBuffer.NumIndices = NumIndices;
		BeginInitResource(&IndexBuffer);
	}


	FProceduralSkeletalMeshSceneProxy(UProceduralSkeletalMeshComponent* Component) :
		FPrimitiveSceneProxy(Component)
		, Material(NULL)
		, VertexFactory(GetScene().GetFeatureLevel(), "FProceduralSkeletalMeshSceneProxy")
		//, DynamicData(NULL)
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		//VertexBuffers.InitWithDummyData(&VertexFactory, LoadedMeshes[0].Vertices.Num());

		//IndexBuffer.NumIndices = GetRequiredIndexCount();

		// Enqueue initialization of render resource
		//BeginInitResource(&IndexBuffer);

		// Grab material
		Material = Component->GetMaterial(0);
		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FProceduralSkeletalMeshSceneProxy() //not sure why this is virtual lol
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();

		if (DynamicData != NULL)
		{
			delete DynamicData;
		}
	}

	void BuildSkeletalMesh(TArray<FDynamicMeshVertex>& OutVertices, TArray<int32>& OutIndices)
	{
		//const FColor VertexColor(255, 255, 255);
		//const int32 NumPoints = InPoints.Num();
		//const int32 SegmentCount = NumPoints - 1;

		for (const auto& mesh : LoadedMeshes)
		{
			for (const auto& vert : mesh.Vertices)
			{
				//FDynamicMeshVertex NewVert;
				OutVertices.Emplace(vert.Location);
			}
			for (const auto& element : mesh.Elements)
				OutIndices.Add(element);
		}
		//// Build vertices

		//// We double up the first and last vert of the ring, because the UVs are different
		//int32 NumRingVerts = NumSides + 1;

		//// For each point along spline..
		//for (int32 PointIdx = 0; PointIdx < NumPoints; PointIdx++)
		//{
		//	const float AlongFrac = (float)PointIdx / (float)SegmentCount; // Distance along cable

		//	// Find direction of cable at this point, by averaging previous and next points
		//	const int32 PrevIndex = FMath::Max(0, PointIdx - 1);
		//	const int32 NextIndex = FMath::Min(PointIdx + 1, NumPoints - 1);
		//	const FVector ForwardDir = (InPoints[NextIndex] - InPoints[PrevIndex]).GetSafeNormal();

		//	// Find quat from up (Z) vector to forward
		//	const FQuat DeltaQuat = FQuat::FindBetween(FVector(0, 0, -1), ForwardDir);

		//	// Apply quat orth vectors
		//	const FVector RightDir = DeltaQuat.RotateVector(FVector(0, 1, 0));
		//	const FVector UpDir = DeltaQuat.RotateVector(FVector(1, 0, 0));

		//	// Generate a ring of verts
		//	for (int32 VertIdx = 0; VertIdx < NumRingVerts; VertIdx++)
		//	{
		//		const float AroundFrac = float(VertIdx) / float(NumSides);
		//		// Find angle around the ring
		//		const float RadAngle = 2.f * PI * AroundFrac;
		//		// Find direction from center of cable to this vertex
		//		const FVector OutDir = (FMath::Cos(RadAngle) * UpDir) + (FMath::Sin(RadAngle) * RightDir);

		//		FDynamicMeshVertex Vert;
		//		Vert.Position = InPoints[PointIdx] + (OutDir * 0.5f * CableWidth);
		//		Vert.TextureCoordinate[0] = FVector2D(AlongFrac * TileMaterial, AroundFrac);
		//		Vert.Color = VertexColor;
		//		Vert.SetTangents(ForwardDir, OutDir ^ ForwardDir, OutDir);
		//		OutVertices.Add(Vert);
		//	}
		//}

		//// Build triangles
		//for (int32 SegIdx = 0; SegIdx < SegmentCount; SegIdx++)
		//{
		//	for (int32 SideIdx = 0; SideIdx < NumSides; SideIdx++)
		//	{
		//		int32 TL = GetVertIndex(SegIdx, SideIdx);
		//		int32 BL = GetVertIndex(SegIdx, SideIdx + 1);
		//		int32 TR = GetVertIndex(SegIdx + 1, SideIdx);
		//		int32 BR = GetVertIndex(SegIdx + 1, SideIdx + 1);

		//		OutIndices.Add(TL);
		//		OutIndices.Add(BL);
		//		OutIndices.Add(TR);

		//		OutIndices.Add(TR);
		//		OutIndices.Add(BL);
		//		OutIndices.Add(BR);
		//	}
		//}
	}

	/** Called on render thread to assign new dynamic data */
	void SetDynamicData_RenderThread(FSkeletonDynamicData* NewDynamicData)
	{
		check(IsInRenderingThread());

		// Free existing data if present
		if (DynamicData)
		{
			delete DynamicData;
			DynamicData = NULL;
		}
		DynamicData = NewDynamicData;

		// Build mesh
		TArray<FDynamicMeshVertex> Vertices;
		TArray<int32> Indices;
		BuildSkeletalMesh(Vertices, Indices);

		//check(Vertices.Num() == GetRequiredVertexCount());
		//check(Indices.Num() == GetRequiredIndexCount());

		for (int i = 0; i < Vertices.Num(); i++)
		{
			const FDynamicMeshVertex& Vertex = Vertices[i];

			VertexBuffers.PositionVertexBuffer.VertexPosition(i) = Vertex.Position;
			VertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(i, Vertex.TangentX.ToFVector(), Vertex.GetTangentY(), Vertex.TangentZ.ToFVector());
			VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(i, 0, Vertex.TextureCoordinate[0]);
			VertexBuffers.ColorVertexBuffer.VertexColor(i) = Vertex.Color;
		}

		{
			auto& VertexBuffer = VertexBuffers.PositionVertexBuffer;
			void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetNumVertices() * VertexBuffer.GetStride(), RLM_WriteOnly);
			FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetNumVertices() * VertexBuffer.GetStride());
			RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
		}

		{
			auto& VertexBuffer = VertexBuffers.ColorVertexBuffer;
			void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetNumVertices() * VertexBuffer.GetStride(), RLM_WriteOnly);
			FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), VertexBuffer.GetNumVertices() * VertexBuffer.GetStride());
			RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
		}

		{
			auto& VertexBuffer = VertexBuffers.StaticMeshVertexBuffer;
			void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTangentSize(), RLM_WriteOnly);
			FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTangentData(), VertexBuffer.GetTangentSize());
			RHIUnlockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI);
		}

		{
			auto& VertexBuffer = VertexBuffers.StaticMeshVertexBuffer;
			void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
			FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTexCoordData(), VertexBuffer.GetTexCoordSize());
			RHIUnlockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);
		}

		void* IndexBufferData = RHILockIndexBuffer(IndexBuffer.IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(IndexBufferData, &Indices[0], Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBuffer.IndexBufferRHI);
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }
};

void UProceduralSkeletalMeshComponent::LoadMeshData(TArray<SkeletalMeshData>& data)
{
	FProceduralSkeletalMeshSceneProxy* SkeletalMeshSceneProxy = (FProceduralSkeletalMeshSceneProxy*)SceneProxy;
	SkeletalMeshSceneProxy->InitBuffersWithSize(data[0].Vertices.Num(), data[0].Elements.Num());
	for (const auto& mesh : data)
	{
		SkeletalMeshSceneProxy->LoadedMeshes.Add(mesh);
	}
}

void UProceduralSkeletalMeshComponent::OnRegister()
{
	Super::OnRegister();

}

void UProceduralSkeletalMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UProceduralSkeletalMeshComponent::SendRenderDynamicData_Concurrent()
{
	if (SceneProxy)
	{
		// Allocate cable dynamic data
		FSkeletonDynamicData* DynamicData = new FSkeletonDynamicData;

		// Transform current positions from particles into component-space array
		//const FTransform& ComponentTransform = GetComponentTransform();
		//int32 NumPoints = NumSegments + 1;
		//DynamicData->CablePoints.AddUninitialized(NumPoints);
		//for (int32 PointIdx = 0; PointIdx < NumPoints; PointIdx++)
		//{
		//	DynamicData->CablePoints[PointIdx] = ComponentTransform.InverseTransformPosition(Particles[PointIdx].Position);
		//}

		// Enqueue command to send to render thread
		FProceduralSkeletalMeshSceneProxy* SkeletalMeshSceneProxy = (FProceduralSkeletalMeshSceneProxy*)SceneProxy;
		//ENQUEUE_RENDER_COMMAND(FSendCableDynamicData)(
		//	[SkeletalMeshSceneProxy, DynamicData](FRHICommandListImmediate& RHICmdList)
		//	{
		//		CableSceneProxy->SetDynamicData_RenderThread(DynamicData);
		//	});

		ENQUEUE_RENDER_COMMAND(FSendSkeletalDynamicData)(
			[SkeletalMeshSceneProxy, DynamicData](FRHICommandListImmediate& RHICmdList)
			{
				SkeletalMeshSceneProxy->SetDynamicData_RenderThread(DynamicData);
			});
	}
}

void UProceduralSkeletalMeshComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	Super::CreateRenderState_Concurrent(Context);

	SendRenderDynamicData_Concurrent();
}

FBoxSphereBounds UProceduralSkeletalMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds();
}

FPrimitiveSceneProxy* UProceduralSkeletalMeshComponent::CreateSceneProxy()
{
	return new FProceduralSkeletalMeshSceneProxy(this);
}

int32 UProceduralSkeletalMeshComponent::GetNumMaterials() const
{
	return 1; //change this
}
