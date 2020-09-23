// Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpInterface.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

Assimp::Importer importer;

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
            const auto& Vert = &mesh->mVertices[t];
            const auto& Normal = &mesh->mNormals[t];
            const auto& Tangent = &mesh->mTangents[t];
            FVector2D TexCoords(0.0f, 0.0f);
            if (mesh->HasTextureCoords(0))
            {
                const auto AITexCoords = &mesh->mTextureCoords[0][t];
                TexCoords = FVector2D(AITexCoords->x, AITexCoords->y); // 1-?
            }
            NewMesh->Vertices.Emplace(AIVEC3_TO_FVEC(Vert), AIVEC3_TO_FVEC(Normal), AIVEC3_TO_FVEC(Tangent), TexCoords);
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
	auto FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/icosphere.fbx";
	UE_LOG(LogTemp, Display, TEXT("Importing FBX file %s"), *FileDir);

	const aiScene* scene = importer.ReadFile(TCHAR_TO_UTF8(*FileDir),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
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

}
