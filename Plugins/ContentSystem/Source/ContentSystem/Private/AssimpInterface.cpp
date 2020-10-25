// Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpInterface.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "..\Public\AssimpInterface.h"

FMatrix AIMattoFMatrix2(const aiMatrix4x4& mat)
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

FMeshContent UAssimpInterface::ImportFBX(bool PreTransformVerts)
{
    FMeshContent MeshContent;

    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 0xFFFF);
	//const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/other/main.fbx";
	//const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/other/neville/bogey.fbx";
	const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/pb15main/pb15_lod0.fbx";
	//const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/dave/daveanim.fbx";
	//const FString FileDir = FPaths::ProjectPluginsDir() + "ContentSystem/Content/Samples/other/duncan/bogey.fbx";
    const FString FilePath = FPaths::GetPath(FileDir);
    MeshContent.DataPath = FilePath + TEXT("/");

	UE_LOG(LogTemp, Display, TEXT("Importing FBX file %s"), *FileDir);

    unsigned int flags = aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_SplitLargeMeshes; //16 bit indices

    if (PreTransformVerts) flags |= aiProcess_PreTransformVertices;

	const aiScene* scene = importer.ReadFile(TCHAR_TO_UTF8(*FileDir), flags);
    //aiProcess_MakeLeftHanded |
    //aiProcess_FlipWindingOrder |
    //aiProcess_FlipUVs |
	if (!scene)
	{
		UE_LOG(LogTemp, Error, TEXT("File import failed! %s"), UTF8_TO_TCHAR(importer.GetErrorString()));
		return MeshContent;
	}

    int upAxis = 2;
    scene->mMetaData->Get<int>("UpAxis", upAxis);
    int upAxisSign = 1;
    scene->mMetaData->Get<int>("UpAxisSign", upAxisSign);

    int frontAxis = 1;
    scene->mMetaData->Get<int>("FrontAxis", frontAxis);
    int frontAxisSign = 1;
    scene->mMetaData->Get<int>("FrontAxisSign", frontAxisSign);

    int coordAxis = 0;
    scene->mMetaData->Get<int>("CoordAxis", coordAxis);
    int coordAxisSign = 1;
    scene->mMetaData->Get<int>("CoordAxisSign", coordAxisSign);

    aiVector3D upVec = upAxis == 0 ? aiVector3D(upAxisSign, 0, 0) : upAxis == 1 ? aiVector3D(0, upAxisSign, 0) : aiVector3D(0, 0, upAxisSign);
    aiVector3D forwardVec = frontAxis == 0 ? aiVector3D(frontAxisSign, 0, 0) : frontAxis == 1 ? aiVector3D(0, frontAxisSign, 0) : aiVector3D(0, 0, frontAxisSign);
    aiVector3D rightVec = coordAxis == 0 ? aiVector3D(coordAxisSign, 0, 0) : coordAxis == 1 ? aiVector3D(0, coordAxisSign, 0) : aiVector3D(0, 0, coordAxisSign);
    aiMatrix4x4 mat(rightVec.x, rightVec.y, rightVec.z, 0.0f,
        upVec.x, upVec.y, upVec.z, 0.0f,
        forwardVec.x, forwardVec.y, forwardVec.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

	UE_LOG(LogTemp, Display, TEXT("Import sucessful."));

    RailwaysImportData* MeshData = new RailwaysImportData();
    MeshContent.MeshData = MeshData;

    //MeshData->RootNode = new AssimpNode(FilePath, MeshData->m_BoneMapping, MeshData->Bones, scene, scene->mRootNode, FTransform(AIMattoFMatrix(mat))); //AIMattoFMatrix(mat)
    FMatrix Identity = FMatrix::Identity;
    MeshData->RootNode = RailwaysNode(FilePath, MeshData->m_BoneMapping, MeshData->Bones, scene, scene->mRootNode, FTransform(Identity)); //AIMattoFMatrix(mat)
    //MeshData->RootNode = new AssimpNode(FilePath, MeshData->m_BoneMapping, MeshData->Bones, scene, scene->mRootNode, FTransform(AIMattoFMatrix(mat))); //AIMattoFMatrix(mat)
    
    //process materials
    MeshData->Materials.AddDefaulted(scene->mNumMaterials);
    for (uint32 i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* Material = scene->mMaterials[i];
        RailwaysMaterial& NewMaterial = MeshData->Materials[i];

        UE_LOG(LogTemp, Display, TEXT("loading %u textures"), Material->GetTextureCount(aiTextureType_DIFFUSE));
        aiString path;
        if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
        {
            //FString TexPath = FilePath + TEXT("/") + FPaths::GetPathLeaf();
            FString TexPath = FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMaterial.Textures.Emplace(TextureType::Diffuse, TexPath);
            UE_LOG(LogTemp, Log, TEXT("Loaded diffuse texture %s"), *TexPath);
        }
        if (Material->GetTexture(aiTextureType_NORMALS, 0, &path) == aiReturn_SUCCESS)
        {
            FString TexPath = FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMaterial.Textures.Emplace(TextureType::Normal, TexPath);
        }
        if (Material->GetTexture(aiTextureType_SHININESS, 0, &path) == aiReturn_SUCCESS)
        {
            FString TexPath = FPaths::GetPathLeaf(UTF8_TO_TCHAR(path.C_Str()));
            NewMaterial.Textures.Emplace(TextureType::Parameter, TexPath);
        }
    }

    //MeshData->RootNode->ProcessBoneHierarchy(MeshData->m_BoneMapping, MeshData->Bones, FTransform(AIMattoFMatrix(mat)), 0);
    MeshData->RootNode.ProcessBoneHierarchy(MeshData->m_BoneMapping, MeshData->Bones, AIMattoFMatrix2(scene->mRootNode->mTransformation), INDEX_NONE);
    //AssimpImportData* MeshData = new AssimpImportData(new AssimpNode(FilePath, scene, scene->mRootNode));
    //ProcessNode(FilePath, scene, scene->mRootNode, MeshData->RootNode);

    for (uint32 i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* anim = scene->mAnimations[i];
        //AssimpAnimation& NewAnimation = *new(MeshData->Animations) AssimpAnimation();
        //NewAnimation.Name = UTF8_TO_TCHAR(anim->mName.C_Str());
        FString AnimName = UTF8_TO_TCHAR(anim->mName.C_Str());

        RailwaysAnimMetadata& NewData = *new(MeshData->AnimData) RailwaysAnimMetadata();
        NewData.Duration = anim->mDuration / ((anim->mTicksPerSecond) ? anim->mTicksPerSecond : 30);

        for (uint32 t = 0; t < anim->mNumChannels; t++)
        {
            aiNodeAnim* nodeAnim = anim->mChannels[t];
            FString NodeName = UTF8_TO_TCHAR(nodeAnim->mNodeName.C_Str());
            if (MeshData->m_BoneMapping.Contains(NodeName))
            {
                uint32 AffectedBoneIdx = MeshData->m_BoneMapping[NodeName];
                RailwaysBone& AffectedBone = MeshData->Bones[AffectedBoneIdx];

                RailwaysAnimationTrack& NewTrack = *new(AffectedBone.AnimTracks) RailwaysAnimationTrack();
                NewTrack.Name = AnimName;

                //TSortedMap<double, FVector> PositionFrames;
                //TSortedMap<double, FQuat> RotationFrames;
                //TSortedMap<double, FVector> ScaleFrames;
                for (uint32 key = 0; key < nodeAnim->mNumPositionKeys; key++)
                {
                    aiVectorKey Frame = nodeAnim->mPositionKeys[key];
                    NewTrack.PosFrames.Add(AIVEC3REF_TO_FVEC(Frame.mValue));
                }
                for (uint32 key = 0; key < nodeAnim->mNumRotationKeys; key++)
                {
                    aiQuatKey Frame = nodeAnim->mRotationKeys[key];
                    NewTrack.RotFrames.Add(AIQUATREF_TO_FQUAT(Frame.mValue));
                }
                for (uint32 key = 0; key < nodeAnim->mNumScalingKeys; key++)
                {
                    aiVectorKey Frame = nodeAnim->mScalingKeys[key];
                    NewTrack.ScaleFrames.Add(AIVEC3REF_TO_FVEC(Frame.mValue));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to find bone %s"), *NodeName);
            }

            //nodeAnim->mNodeName
        }
        UE_LOG(LogTemp, Warning, TEXT("loaded animation %s"), *AnimName);
    }


    return MeshContent;
	//return MeshData;
}