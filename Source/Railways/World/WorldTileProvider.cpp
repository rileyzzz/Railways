// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldTileProvider.h"
#include "WorldTileDynamic.h"
#include "DrawDebugHelpers.h"

UMaterialInterface* UWorldTileProvider::GetTileMaterial() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return Material;
}

void UWorldTileProvider::SetTileMaterial(UMaterialInterface* InMaterial)
{
	FScopeLock Lock(&PropertySyncRoot);
	Material = InMaterial;
	SetupMaterialSlot(0, FName("Material"), Material);
}

void UWorldTileProvider::SetTileParent(AWorldTileDynamic* InParent)
{
	FScopeLock Lock(&PropertySyncRoot);
	Tile = InParent;
}

bool UWorldTileProvider::WithinBounds(int x, int y)
{
	if (x >= WORLD_SIZE) return false;
	if (y >= WORLD_SIZE) return false;
	if (x < 0) return false;
	if (y < 0) return false;
	return true;
}

void UWorldTileProvider::InvalidateMeshData()
{
	MarkAllLODsDirty();
	MarkCollisionDirty();
}

//UWorldTileProvider::UWorldTileProvider()/* : URuntimeMeshProvider()*/
//{
//
//}
//
//UWorldTileProvider::~UWorldTileProvider()
//{
//	
//}

//#define MAXLOD 5
#define MAXLOD 5

void UWorldTileProvider::Initialize()
{
	//f_heightData = new float* [WORLD_SIZE];
	//for (unsigned int i = 0; i < WORLD_SIZE; i++)
	//	f_heightData[i] = new float[WORLD_SIZE];

	//for (unsigned int x = 0; x < WORLD_SIZE; x++)
	//{
	//	for (unsigned int y = 0; y < WORLD_SIZE; y++)
	//	{
	//		f_heightData[x][y] = 0.0f;
	//	}
	//}


	/*FRuntimeMeshLODProperties LODProperties;
	LODProperties.ScreenSize = 0.0f;
	ConfigureLODs({ LODProperties });*/

	TArray<FRuntimeMeshLODProperties> LODs;

	for (int LOD = 0; LOD < MAXLOD; LOD++)
	{
		//float alpha = (float)LOD / (float)MAXLOD;

		FRuntimeMeshLODProperties LODProperties;
		LODProperties.ScreenSize = FMath::Pow(0.75, LOD);
		LODs.Add(LODProperties);
	}
	ConfigureLODs(LODs);

	SetupMaterialSlot(0, FName("Material"), Material);

	for (int LOD = 0; LOD < MAXLOD; LOD++)
	{
		FRuntimeMeshSectionProperties Properties;
		Properties.bCastsShadow = true;
		Properties.bIsVisible = true;
		Properties.MaterialSlot = 0;
		Properties.bWants32BitIndices = true;
		Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Frequent; //potential crash source?
		static int SectionID = 0;
		CreateSection(LOD, SectionID++, Properties);
	}
	//CreateSection(0, 0, Properties);
	//MarkAllLODsDirty();
	MarkCollisionDirty();
}

FBoxSphereBounds UWorldTileProvider::GetBounds()
{
	//causes artifacts on a blank tile if 0
	float MinHeight = -0.1f;
	float MaxHeight = 0.1f;

	for (unsigned int x = 0; x < WORLD_SIZE; x++)
	{
		for (unsigned int y = 0; y < WORLD_SIZE; y++)
		{
			float Height = Tile->Terrain.GetHeight(x, y);
			MinHeight = FMath::Min(MinHeight, Height);
			MaxHeight = FMath::Max(MaxHeight, Height);
		}
	}
	constexpr float BoundsSize = WORLD_SIZE * WORLD_SCALE;
	FVector Start(0.0f, 0.0f, MinHeight);
	FVector End(BoundsSize, BoundsSize, MaxHeight);
	//FBox Box = FBox(-Ext / 2, Ext / 2);
	//FBox Box = FBox(FVector(0.0f, 0.0f, -10000.0f), Ext);
	//FBox Box(Start, End);
	//return FBoxSphereBounds(Box);
	return FBoxSphereBounds(FBox(Start, End));
}

bool UWorldTileProvider::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	//check(LODIndex == 0 && SectionId == 0);
	FScopeLock Lock(&PropertySyncRoot);
	const unsigned int Simplify = (LODIndex * 1 + 1);
	//int dx = 1; //change of x between two points
	//int dy = 1; //change of y between two points
	
	//for (unsigned int y = 0; y < WORLD_SIZE; y++)
	//{
	//	float yalpha = (float)y / (float)(WORLD_SIZE - 1);
	//	//float ypos = FMath::Lerp(-WORLD_SIZE / 2, WORLD_SIZE / 2, yalpha);
	//	for (unsigned int x = 0; x < WORLD_SIZE; x++)
	//	{
	//		float xalpha = (float)x / (float)(WORLD_SIZE - 1);
	//		//float xpos = FMath::Lerp(-WORLD_SIZE / 2, WORLD_SIZE / 2, xalpha);

	//		//float height = GetHeight(x, y);

	//		FVector Position((float)x * WORLD_SCALE, (float)y * WORLD_SCALE, GetHeight(x, y));
	//		//UE_LOG(LogTemp, Warning, TEXT("created vert at %f %f %f"), Position.X, Position.Y, Position.Z);
	//		float dfdx = (GetHeight(x + dx, y) - GetHeight(x - dx, y)) / (2 * (float)dx); //derivative of f over x
	//		float dfdy = (GetHeight(x, y + dy) - GetHeight(x, y - dy)) / (2 * (float)dy); //derivative of f over y

	//		//float dfdx = 0.0f; //derivative of f over x
	//		//float dfdy = 0.0f; //derivative of f over y

	//		FVector Normal(-dfdx, -dfdy, 1);
	//		Normal.Normalize();
	//		FVector Tangent(1, 0, dfdx);
	//		Tangent.Normalize();

	//		FVector2D UV(xalpha, yalpha);

	//		MeshData.Positions.Add(Position);
	//		MeshData.Tangents.Add(Normal, Tangent);
	//		MeshData.TexCoords.Add(UV);
	//		MeshData.Colors.Add(FColor::White);

	//		if (x != WORLD_SIZE - 1 && y != WORLD_SIZE - 1)
	//		{
	//			int32 AIndex = x + y * WORLD_SIZE;
	//			int32 BIndex = AIndex + 1;
	//			int32 CIndex = AIndex + WORLD_SIZE;
	//			int32 DIndex = CIndex + 1;
	//			MeshData.Triangles.AddTriangle(AIndex, CIndex, BIndex);
	//			MeshData.Triangles.AddTriangle(BIndex, CIndex, DIndex);
	//		}
	//	}
	//}


	int dx = Simplify; //change of x between two points
	int dy = Simplify; //change of y between two points

	const unsigned int SimplifySize = WORLD_SIZE / Simplify;
	for (unsigned int y = 0; y < SimplifySize; y++)
	{
		float yalpha = (float)y / (float)(SimplifySize - 1);

		for (unsigned int x = 0; x < SimplifySize; x++)
		{
			float xalpha = (float)x / (float)(SimplifySize - 1);

			/*int PointX = x * Simplify;
			int PointY = y * Simplify;*/
			float PointX = x * ((float)(WORLD_SIZE - 1) / (float)(SimplifySize - 1));
			float PointY = y * ((float)(WORLD_SIZE - 1) / (float)(SimplifySize - 1));
			FVector Position(PointX * WORLD_SCALE, PointY * WORLD_SCALE, Tile->Terrain.GetHeight(PointX, PointY));

			float dfdx = (Tile->Terrain.GetHeight(PointX + dx, PointY) - Tile->Terrain.GetHeight(PointX - dx, PointY)) / (2 * (float)dx); //derivative of f over x
			float dfdy = (Tile->Terrain.GetHeight(PointX, PointY + dy) - Tile->Terrain.GetHeight(PointX, PointY - dy)) / (2 * (float)dy); //derivative of f over y


			FVector Normal(-dfdx, -dfdy, 1);
			Normal.Normalize();
			FVector Tangent(1, 0, dfdx);
			Tangent.Normalize();

			FVector PointLoc = Tile->GetActorLocation() + Position;
			//DrawDebugLine(GetWorld(), PointLoc, PointLoc + Normal * 100.0f, FColor::Blue, false);
			//DrawDebugLine(GetWorld(), PointLoc, PointLoc + Tangent * 100.0f, FColor::Orange, false);
			FVector2D UV(xalpha, yalpha);

			MeshData.Positions.Add(Position);
			MeshData.Tangents.Add(Normal, Tangent);
			MeshData.TexCoords.Add(UV);
			MeshData.Colors.Add(FColor::White);

			if (x != SimplifySize - 1 && y != SimplifySize - 1)
			{
				uint32 AIndex = x + y * SimplifySize;
				uint32 BIndex = AIndex + 1;
				uint32 CIndex = AIndex + SimplifySize;
				uint32 DIndex = CIndex + 1;

				//auto SharedEdge1 = glm::length(vTopRight - vBottomLeft);
				//auto SharedEdge2 = glm::length(vBottomRight - vTopLeft);
				float AHeight = Tile->Terrain.GetHeight(PointX, PointY);
				float BHeight = Tile->Terrain.GetHeight(PointX + Simplify, PointY);
				float CHeight = Tile->Terrain.GetHeight(PointX, PointY + Simplify);
				float DHeight = Tile->Terrain.GetHeight(PointX + Simplify, PointY + Simplify);

				float SharedEdge1 = BHeight - CHeight;
				float SharedEdge2 = DHeight - AHeight;

				if (FMath::Abs(SharedEdge2) < FMath::Abs(SharedEdge1))
				{
					MeshData.Triangles.AddTriangle(AIndex, CIndex, DIndex);
					MeshData.Triangles.AddTriangle(AIndex, DIndex, BIndex);
				}
				else
				{
					MeshData.Triangles.AddTriangle(AIndex, CIndex, BIndex);
					MeshData.Triangles.AddTriangle(BIndex, CIndex, DIndex);
				}
			}
		}
	}


	return true;
}

FRuntimeMeshCollisionSettings UWorldTileProvider::GetCollisionSettings()
{
	FRuntimeMeshCollisionSettings Settings;
	//Settings.bUseAsyncCooking = false;
	Settings.bUseAsyncCooking = true;
	Settings.bUseComplexAsSimple = true;

	//Settings.Boxes.Emplace(7200.0f, 7200.0f, 7200.0f);

	return Settings;
}

bool UWorldTileProvider::HasCollisionMesh()
{
	return true;
	//return false;
}

bool UWorldTileProvider::GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData)
{
	FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
	FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;

	const unsigned int Simplify = 8;

	const unsigned int SimplifySize = WORLD_SIZE / Simplify;
	for (unsigned int y = 0; y < SimplifySize; y++)
	{
		for (unsigned int x = 0; x < SimplifySize; x++)
		{
			float PointX = x * ((float)(WORLD_SIZE - 1) / (float)(SimplifySize - 1));
			float PointY = y * ((float)(WORLD_SIZE - 1) / (float)(SimplifySize - 1));
			FVector Position(PointX * WORLD_SCALE, PointY * WORLD_SCALE, Tile->Terrain.GetHeight(PointX, PointY));

			CollisionVertices.Add(Position);

			if (x != SimplifySize - 1 && y != SimplifySize - 1)
			{
				uint32 AIndex = x + y * SimplifySize;
				uint32 BIndex = AIndex + 1;
				uint32 CIndex = AIndex + SimplifySize;
				uint32 DIndex = CIndex + 1;

				float AHeight = Tile->Terrain.GetHeight(PointX, PointY);
				float BHeight = Tile->Terrain.GetHeight(PointX + Simplify, PointY);
				float CHeight = Tile->Terrain.GetHeight(PointX, PointY + Simplify);
				float DHeight = Tile->Terrain.GetHeight(PointX + Simplify, PointY + Simplify);

				float SharedEdge1 = BHeight - CHeight;
				float SharedEdge2 = DHeight - AHeight;

				if (FMath::Abs(SharedEdge2) < FMath::Abs(SharedEdge1))
				{
					CollisionTriangles.Add(AIndex, CIndex, DIndex);
					CollisionTriangles.Add(AIndex, DIndex, BIndex);
				}
				else
				{
					CollisionTriangles.Add(AIndex, CIndex, BIndex);
					CollisionTriangles.Add(BIndex, CIndex, DIndex);
				}
			}
		}
	}


	return true;
}

bool UWorldTileProvider::IsThreadSafe()
{
	//return true; potential crash source?
	return false;
}