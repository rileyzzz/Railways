// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldTileProvider.h"
#include "WorldTileDynamic.h"

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
	FScopeLock Lock(&PropertySyncRoot);
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
#define MAXLOD 1

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

	UE_LOG(LogTemp, Warning, TEXT("TILE INIT"));


	/*FRuntimeMeshLODProperties LODProperties;
	LODProperties.ScreenSize = 0.0f;
	ConfigureLODs({ LODProperties });*/

	TArray<FRuntimeMeshLODProperties> LODs;

	for (int LOD = 0; LOD < MAXLOD; LOD++)
	{
		//float alpha = (float)LOD / (float)MAXLOD;

		FRuntimeMeshLODProperties LODProperties;
		LODProperties.ScreenSize = FMath::Pow(0.5, LOD);
		LODs.Add(LODProperties);
	}
	ConfigureLODs(LODs);

	SetupMaterialSlot(0, FName("Material"), GetTileMaterial());

	for (int LOD = 0; LOD < MAXLOD; LOD++)
	{
		FRuntimeMeshSectionProperties Properties;
		Properties.bCastsShadow = true;
		Properties.bIsVisible = true;
		Properties.MaterialSlot = 0;
		Properties.bWants32BitIndices = true;
		Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Frequent;
		CreateSection(LOD, 0, Properties);
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
				MeshData.Triangles.AddTriangle(AIndex, CIndex, BIndex);
				MeshData.Triangles.AddTriangle(BIndex, CIndex, DIndex);
			}
		}
	}


	return true;
}

FRuntimeMeshCollisionSettings UWorldTileProvider::GetCollisionSettings()
{
	FScopeLock Lock(&PropertySyncRoot);
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
	FScopeLock Lock(&PropertySyncRoot);
	constexpr int COLLISION_RESOLUTION = 4;
	constexpr int COLLISION_SIZE = WORLD_SIZE / COLLISION_RESOLUTION + 1;

	constexpr int COLLISION_COUNT = ((COLLISION_SIZE) * (COLLISION_SIZE)) * 2;
	// Add the single collision section
	//CollisionData.CollisionSources.Emplace(0, COLLISION_COUNT, this, 0, ERuntimeMeshCollisionFaceSourceType::Collision);

	FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
	FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;

	for (unsigned int y = 0; y < COLLISION_SIZE; y++)
	{
		for (unsigned int x = 0; x < COLLISION_SIZE; x++)
		{
			float height = Tile->Terrain.GetHeight(x * COLLISION_RESOLUTION, y * COLLISION_RESOLUTION);

			FVector Position((float)x * (float)WORLD_SCALE * (float)COLLISION_RESOLUTION, (float)y * (float)WORLD_SCALE * (float)COLLISION_RESOLUTION, height);

			CollisionVertices.Add(Position);

			if (x != COLLISION_SIZE - 1 && y != COLLISION_SIZE - 1)
			{
				int32 AIndex = x + y * COLLISION_SIZE;
				int32 BIndex = AIndex + 1;
				int32 CIndex = AIndex + COLLISION_SIZE;
				int32 DIndex = CIndex + 1;
				CollisionTriangles.Add(AIndex, CIndex, BIndex);
				CollisionTriangles.Add(BIndex, CIndex, DIndex);
			}
		}
	}


	return true;
}

bool UWorldTileProvider::IsThreadSafe()
{
	return true;
}