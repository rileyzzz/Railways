// Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpInterface.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define AIVEC3_TO_FVEC(vector) FVector(vector->x, vector->y, vector->z)

FMatrix AIMattoFMatrix(const aiMatrix4x4& mat)
{
    FMatrix Ret;
    Ret.M[0][0] = mat.a1;
    Ret.M[0][1] = mat.b1;
    Ret.M[0][2] = mat.c1;
    Ret.M[0][3] = mat.d1;

    Ret.M[1][0] = mat.a2;
    Ret.M[1][1] = mat.b2;
    Ret.M[1][2] = mat.c2;
    Ret.M[1][3] = mat.d2;

    Ret.M[2][0] = mat.a3;
    Ret.M[2][1] = mat.b3;
    Ret.M[2][2] = mat.c3;
    Ret.M[2][3] = mat.d3;

    Ret.M[3][0] = mat.a4;
    Ret.M[3][1] = mat.b4;
    Ret.M[3][2] = mat.c4;
    Ret.M[3][3] = mat.d4;

    return Ret;
}

AssimpNode::AssimpNode(const FString& FilePath, TArray<AssimpBone>& Bones, const aiScene* scene, aiNode* node)
{
    
    Name = UTF8_TO_TCHAR(node->mName.C_Str());
    Transform = AIMattoFMatrix(node->mTransformation);

    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        AssimpMesh* NewMesh = new AssimpMesh();
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        NewMesh->Name = UTF8_TO_TCHAR(mesh->mName.C_Str());

        //mesh data
        for (uint32 t = 0; t < mesh->mNumVertices; t++)
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
            const FVector scalefactor(1.0f, -1.0f, -1.0f);
            //const FVector scalefactor(1.0f, 1.0f, 1.0f);
            
            //NewMesh->Vertices.Emplace(AIVEC3_TO_FVEC(Vert) * scalefactor, AIVEC3_TO_FVEC(Normal) * scalefactor, AIVEC3_TO_FVEC(Tangent) * scalefactor, TexCoords);
            NewMesh->Vertices.Emplace(Transform.TransformFVector4(AIVEC3_TO_FVEC(Vert)) * scalefactor,
                Transform.TransformFVector4(AIVEC3_TO_FVEC(Normal)) * scalefactor,
                Transform.TransformFVector4(AIVEC3_TO_FVEC(Tangent)) * scalefactor,
                TexCoords);
        }

        for (uint32 t = 0; t < mesh->mNumFaces; t++)
        {
            const aiFace* face = &mesh->mFaces[t];
            for (unsigned int j = 0; j < face->mNumIndices; j++)
                NewMesh->Elements.Add(face->mIndices[j]);
        }

        aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];


        UE_LOG(LogTemp, Display, TEXT("loading %u textures"), Material->GetTextureCount(aiTextureType_DIFFUSE));
        aiString path;
        if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
        {
            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMesh->Material.Textures.Emplace(TextureType::Diffuse, TexPath);
            UE_LOG(LogTemp, Log, TEXT("Loaded diffuse texture %s"), *TexPath);
        }
        if (Material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
        {
            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMesh->Material.Textures.Emplace(TextureType::Normal, TexPath);
        }
        if (Material->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
        {
            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMesh->Material.Textures.Emplace(TextureType::Parameter, TexPath);
        }

        for (uint32 t = 0; t < mesh->mNumBones; t++)
        {
            const aiBone* bone = mesh->mBones[t];
            AssimpBone NewBone;
            NewBone.Name = UTF8_TO_TCHAR(bone->mName.C_Str());
            NewBone.OffsetTransform = AIMattoFMatrix(bone->mOffsetMatrix);
            
            const int BoneIndex = Bones.Num();
            for (uint32 w = 0; w < bone->mNumWeights; w++)
            {
                const aiVertexWeight& weight = bone->mWeights[w];
                AssimpVert& TargetVertex = NewMesh->Vertices[weight.mVertexId];

                TargetVertex.InfluenceBones.Add(BoneIndex);
                TargetVertex.InfluenceWeights.Add((uint8)(weight.mWeight * 255.0f));

                //NewMesh->Vertices[weight.mVertexId].InfluenceWeights[] = weight.mWeight;
            }
            UE_LOG(LogTemp, Display, TEXT("Loaded bone %s with %i weights"), *NewBone.Name, bone->mNumWeights);
            Bones.Add(NewBone);
        }

        UE_LOG(LogTemp, Display, TEXT("Loaded mesh %s with %i verts and %i triangles"), *NewMesh->Name, NewMesh->Vertices.Num(), NewMesh->Elements.Num() / 3);
        Meshes.Add(NewMesh);
    }

    for (uint32 i = 0; i < node->mNumChildren; i++)
        Children.Emplace(FilePath, Bones, scene, node->mChildren[i]);

    //ProcessNode(FilePath, scene, node->mChildren[i], NewNode);
}

//void UAssimpInterface::ProcessNode(const FString& FilePath, const aiScene* scene, aiNode* node, AssimpNode& Parent)
//{
//    AssimpNode NewNode;
//    NewNode.Name = UTF8_TO_TCHAR(node->mName.C_Str());
//    NewNode.Transform = AIMattoFMatrix(node->mTransformation);
//
//    for (unsigned int i = 0; i < node->mNumMeshes; i++)
//    {
//        AssimpMesh* NewMesh = new AssimpMesh();
//        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//        NewMesh->Name = UTF8_TO_TCHAR(mesh->mName.C_Str());
//        
//        //mesh data
//        for (unsigned int t = 0; t < mesh->mNumVertices; t++)
//        {
//            const auto& Vert = &mesh->mVertices[t];
//            const auto& Normal = &mesh->mNormals[t];
//            const auto& Tangent = &mesh->mTangents[t];
//            FVector2D TexCoords(0.0f, 0.0f);
//            if (mesh->HasTextureCoords(0))
//            {
//                const auto AITexCoords = &mesh->mTextureCoords[0][t];
//                TexCoords = FVector2D(AITexCoords->x, AITexCoords->y); // 1-?
//            }
//            const FVector scalefactor(1.0f, -1.0f, -1.0f);
//            //const FVector scalefactor(1.0f, 1.0f, 1.0f);
//            NewMesh->Vertices.Emplace(AIVEC3_TO_FVEC(Vert) * scalefactor, AIVEC3_TO_FVEC(Normal) * scalefactor, AIVEC3_TO_FVEC(Tangent) * scalefactor, TexCoords);
//            //NewMesh->Vertices.Emplace(FVector(Vert->x, Vert->y, Vert->z * -1.0f), AIVEC3_TO_FVEC(Normal), AIVEC3_TO_FVEC(Tangent), TexCoords);
//        }
//
//        for (unsigned int t = 0; t < mesh->mNumFaces; t++)
//        {
//            const aiFace* face = &mesh->mFaces[t];
//            for (unsigned int j = 0; j < face->mNumIndices; j++)
//                NewMesh->Elements.Add(face->mIndices[j]);
//        }
//
//        aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];
//
//        //int texIndex = 0;
//        //aiReturn texFound = AI_SUCCESS;
//
//        //TMap Textures<FString>
//        /*aiString path;
//        while (texFound == AI_SUCCESS)
//        {
//            texFound = Material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
//            texIndex++;
//        }*/
//
//        UE_LOG(LogTemp, Display, TEXT("loading %u textures"), Material->GetTextureCount(aiTextureType_DIFFUSE));
//        aiString path;
//        if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
//        {
//            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
//            NewMesh->Material.Textures.Emplace(TextureType::Diffuse, TexPath);
//            UE_LOG(LogTemp, Log, TEXT("Loaded diffuse texture %s"), *TexPath);
//        }
//        if (Material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
//        {
//            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
//            NewMesh->Material.Textures.Emplace(TextureType::Normal, TexPath);
//        }
//        if (Material->GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
//        {
//            FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
//            NewMesh->Material.Textures.Emplace(TextureType::Parameter, TexPath);
//        }
//
//        //material data
//        //ApplyMaterial(NewMesh, scene->mMaterials[mesh->mMaterialIndex]);
//
//        UE_LOG(LogTemp, Display, TEXT("Loaded mesh %s with %i verts and %i triangles"), *NewMesh->Name, NewMesh->Vertices.Num(), NewMesh->Elements.Num() / 3);
//        NewNode.Meshes.Add(NewMesh);
//        //Meshes.push_back(NewMesh);
//    }
//
//    for (unsigned int i = 0; i < node->mNumChildren; i++)
//        ProcessNode(FilePath, scene, node->mChildren[i], NewNode);
//
//    Parent.Children.Add(NewNode);
//}

AssimpImportData* UAssimpInterface::ImportFBX(bool PreTransformVerts)
{
    Assimp::Importer importer;

	const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/pb15/main.fbx";
    const FString FilePath = FPaths::GetPath(FileDir);
	UE_LOG(LogTemp, Display, TEXT("Importing FBX file %s"), *FileDir);

    unsigned int flags = aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipUVs |
        //aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals;

    if (PreTransformVerts) flags |= aiProcess_PreTransformVertices;

	const aiScene* scene = importer.ReadFile(TCHAR_TO_UTF8(*FileDir), flags);
    //aiProcess_MakeLeftHanded |
    //aiProcess_FlipWindingOrder |
    //aiProcess_FlipUVs |
	if (!scene)
	{
		UE_LOG(LogTemp, Error, TEXT("File import Failed! %s"), UTF8_TO_TCHAR(importer.GetErrorString()));
		return nullptr;
	}

	UE_LOG(LogTemp, Display, TEXT("Import sucessful."));

    AssimpImportData* MeshData = new AssimpImportData();
    MeshData->RootNode = new AssimpNode(FilePath, MeshData->Bones, scene, scene->mRootNode);

    //AssimpImportData* MeshData = new AssimpImportData(new AssimpNode(FilePath, scene, scene->mRootNode));
    //ProcessNode(FilePath, scene, scene->mRootNode, MeshData->RootNode);

	return MeshData;
}