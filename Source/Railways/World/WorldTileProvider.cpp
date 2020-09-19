// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldTileProvider.h"

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

void UWorldTileProvider::SetHeightData(int x, int y, float height)
{
	f_heightData[x][y] = height;
}

void UWorldTileProvider::AddHeight(int x, int y, float height)
{
	f_heightData[x][y] += height;
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

void UWorldTileProvider::Initialize()
{
	f_heightData = new float* [WORLD_SIZE];
	for (unsigned int i = 0; i < WORLD_SIZE; i++)
		f_heightData[i] = new float[WORLD_SIZE];

	for (unsigned int x = 0; x < WORLD_SIZE; x++)
	{
		for (unsigned int y = 0; y < WORLD_SIZE; y++)
		{
			f_heightData[x][y] = 0.0f;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("TILE INIT"));

	FRuntimeMeshLODProperties LODProperties;
	LODProperties.ScreenSize = 0.0f;
	ConfigureLODs({ LODProperties });

	SetupMaterialSlot(0, FName("Material"), GetTileMaterial());

	FRuntimeMeshSectionProperties Properties;
	Properties.bCastsShadow = true;
	Properties.bIsVisible = true;
	Properties.MaterialSlot = 0;
	Properties.bWants32BitIndices = true;
	Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Frequent;
	CreateSection(0, 0, Properties);

	MarkCollisionDirty();
}

FBoxSphereBounds UWorldTileProvider::GetBounds()
{
	FVector Ext(7200.0f, 7200.0f, 7200.0f);
	//FBox Box = FBox(-Ext / 2, Ext / 2);
	FBox Box = FBox(FVector(0.0f, 0.0f, 0.0f), Ext);
	return FBoxSphereBounds(Box);
}

float UWorldTileProvider::GetHeight(int x, int y)
{
	if (x >= WORLD_SIZE) x = WORLD_SIZE - 1;
	if (y >= WORLD_SIZE) y = WORLD_SIZE - 1;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	return f_heightData[x][y];
}

bool UWorldTileProvider::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	//UE_LOG(LogTemp, Warning, TEXT("GETTING LOD SECTION"));
	check(LODIndex == 0 && SectionId == 0);
	int dx = 1; //change of x between two points
	int dy = 1; //change of y between two points
	
	for (unsigned int y = 0; y < WORLD_SIZE; y++)
	{
		float yalpha = (float)y / (float)(WORLD_SIZE - 1);
		//float ypos = FMath::Lerp(-WORLD_SIZE / 2, WORLD_SIZE / 2, yalpha);
		for (unsigned int x = 0; x < WORLD_SIZE; x++)
		{
			float xalpha = (float)x / (float)(WORLD_SIZE - 1);
			//float xpos = FMath::Lerp(-WORLD_SIZE / 2, WORLD_SIZE / 2, xalpha);

			//float height = GetHeight(x, y);

			FVector Position((float)x * WORLD_SCALE, (float)y * WORLD_SCALE, GetHeight(x, y));
			//UE_LOG(LogTemp, Warning, TEXT("created vert at %f %f %f"), Position.X, Position.Y, Position.Z);
			float dfdx = (GetHeight(x + dx, y) - GetHeight(x - dx, y)) / (2 * (float)dx); //derivative of f over x
			float dfdy = (GetHeight(x, y + dy) - GetHeight(x, y - dy)) / (2 * (float)dy); //derivative of f over y

			//float dfdx = 0.0f; //derivative of f over x
			//float dfdy = 0.0f; //derivative of f over y

			FVector Normal(-dfdx, -dfdy, 1);
			Normal.Normalize();
			FVector Tangent(1, 0, dfdx);
			Tangent.Normalize();

			FVector2D UV(xalpha, yalpha);

			MeshData.Positions.Add(Position);
			MeshData.Tangents.Add(Normal, Tangent);
			MeshData.TexCoords.Add(UV);
			MeshData.Colors.Add(FColor::White);

			if (x != WORLD_SIZE - 1 && y != WORLD_SIZE - 1)
			{
				int32 AIndex = x + y * WORLD_SIZE;
				int32 BIndex = AIndex + 1;
				int32 CIndex = AIndex + WORLD_SIZE;
				int32 DIndex = CIndex + 1;
				MeshData.Triangles.AddTriangle(AIndex, CIndex, BIndex);
				MeshData.Triangles.AddTriangle(BIndex, CIndex, DIndex);
			}
		}
	}
	return true;
}

FRuntimeMeshCollisionSettings UWorldTileProvider::GetCollisionSettings()
{
	FRuntimeMeshCollisionSettings Settings;
	Settings.bUseAsyncCooking = false;
	Settings.bUseComplexAsSimple = true;

	//Settings.Boxes.Emplace(7200.0f, 7200.0f, 7200.0f);

	return Settings;
}

bool UWorldTileProvider::HasCollisionMesh()
{
	return true;
}

bool UWorldTileProvider::GetCollisionMesh(FRuntimeMeshCollisionData& CollisionData)
{
	constexpr int COLLISION_RESOLUTION = 10;
	constexpr int COLLISION_SIZE = WORLD_SIZE / COLLISION_RESOLUTION;

	constexpr int COLLISION_COUNT = ((COLLISION_SIZE - 1) * (COLLISION_SIZE - 1)) * 2;
	// Add the single collision section
	CollisionData.CollisionSources.Emplace(0, COLLISION_COUNT, this, 0, ERuntimeMeshCollisionFaceSourceType::Collision);

	FRuntimeMeshCollisionVertexStream& CollisionVertices = CollisionData.Vertices;
	FRuntimeMeshCollisionTriangleStream& CollisionTriangles = CollisionData.Triangles;

	for (unsigned int y = 0; y < COLLISION_SIZE; y++)
	{
		for (unsigned int x = 0; x < COLLISION_SIZE; x++)
		{
			float height = GetHeight(x * COLLISION_RESOLUTION, y * COLLISION_RESOLUTION);

			FVector Position((float)x * WORLD_SCALE * (float)COLLISION_RESOLUTION, (float)y * WORLD_SCALE * (float)COLLISION_RESOLUTION, height);

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