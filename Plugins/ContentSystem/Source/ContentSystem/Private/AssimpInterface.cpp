// Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpInterface.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define AIVEC3_TO_FVEC(vector) FVector(vector->x, vector->y, vector->z)

void ProcessNode(const aiScene* scene, aiNode* node, AssimpImportData* ImportData)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        AssimpMesh* NewMesh = new AssimpMesh();
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        NewMesh->Name = UTF8_TO_TCHAR(mesh->mName.C_Str());
        
        //mesh data
        for (unsigned int t = 0; t < mesh->mNumVertices; t++)
        {
            //const auto& Vert = &mesh->mVertices[t];
            auto Vert = mesh->mVertices[t];
            Vert.x *= 100.0f;
            Vert.y *= 100.0f;
            Vert.z *= 100.0f;

            const auto& Normal = &mesh->mNormals[t];
            const auto& Tangent = &mesh->mTangents[t];
            FVector2D TexCoords(0.0f, 0.0f);
            if (mesh->HasTextureCoords(0))
            {
                const auto AITexCoords = &mesh->mTextureCoords[0][t];
                TexCoords = FVector2D(AITexCoords->x, AITexCoords->y); // 1-?
            }
            NewMesh->Vertices.Emplace(FVector(Vert.x, Vert.y, Vert.z), AIVEC3_TO_FVEC(Normal), AIVEC3_TO_FVEC(Tangent), TexCoords);
        }

        for (unsigned int t = 0; t < mesh->mNumFaces; t++)
        {
            const aiFace* face = &mesh->mFaces[t];
            for (unsigned int j = 0; j < face->mNumIndices; j++)
                NewMesh->Elements.Add(face->mIndices[j]);
        }

        //material data
        //ApplyMaterial(NewMesh, scene->mMaterials[mesh->mMaterialIndex]);

        UE_LOG(LogTemp, Display, TEXT("Loaded mesh %s with %i verts and %i triangles"), *NewMesh->Name, NewMesh->Vertices.Num(), NewMesh->Elements.Num() / 3);
        ImportData->Meshes.Add(NewMesh);
        //Meshes.push_back(NewMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ProcessNode(scene, node->mChildren[i], ImportData);
}

AssimpImportData* UAssimpInterface::ImportFBX()
{
    Assimp::Importer importer;

	auto FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/render2.fbx";
	UE_LOG(LogTemp, Display, TEXT("Importing FBX file %s"), *FileDir);

	const aiScene* scene = importer.ReadFile(TCHAR_TO_UTF8(*FileDir),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder |
		aiProcess_GenNormals);

	if (!scene)
	{
		UE_LOG(LogTemp, Error, TEXT("File import Failed! %s"), UTF8_TO_TCHAR(importer.GetErrorString()));
		return nullptr;
	}

	UE_LOG(LogTemp, Display, TEXT("Import sucessful."));

    AssimpImportData* MeshData = new AssimpImportData();
    ProcessNode(scene, scene->mRootNode, MeshData);

	return MeshData;
}

void UAssimpInterface::BuildComponent(UProceduralSkeletalMeshComponent* Component, AssimpImportData* Data)
{
    const int32 NumMeshes = Data->Meshes.Num();
    for (int i = 0; i < NumMeshes; i++)
    {
        const AssimpMesh* Mesh = Data->Meshes[i];
        TArray<FVector> Positions;
        TArray<FVector> Normals;
        TArray<FVector2D> UVs;
        TArray<FProcSkeletalMeshTangent> Tangents;

        Positions.AddUninitialized(Mesh->Vertices.Num());
        Normals.AddUninitialized(Mesh->Vertices.Num());
        UVs.AddUninitialized(Mesh->Vertices.Num());
        Tangents.AddUninitialized(Mesh->Vertices.Num());

        for (int vert = 0; vert < Mesh->Vertices.Num(); vert++)
        {
            const AssimpVert& VertData = Mesh->Vertices[vert];
            Positions[vert] = VertData.Location;
            Normals[vert] = VertData.Normal;
            UVs[vert] = VertData.TexCoords;
            Tangents[vert] = FProcSkeletalMeshTangent(VertData.Tangent, false);
        }

        TArray<int32> Triangles;
        Triangles.AddUninitialized(Mesh->Elements.Num());
        for (int tri = 0; tri < Mesh->Elements.Num(); tri++)
        {
            Triangles[tri] = Mesh->Elements[tri];
        }

        Component->CreateMeshSection(i, Positions, Triangles, Normals, UVs, Tangents);
    }
    
    //TArray<SkeletalMeshData> newdata;
    //for (const auto& mesh : Data->Meshes)
    //{
    //    SkeletalMeshData skeletalmesh;
    //    for (const auto& vert : mesh->Vertices)
    //        skeletalmesh.Vertices.Emplace(vert.Location, vert.Normal, vert.Tangent, vert.TexCoords);
    //    skeletalmesh.Elements = mesh->Elements;
    //    newdata.Add(skeletalmesh);
    //}

    //Component->LoadMeshData(newdata);
}
