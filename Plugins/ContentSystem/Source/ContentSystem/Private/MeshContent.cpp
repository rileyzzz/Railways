// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshContent.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

    /*Ret.M[0][0] = mat.a1;
    Ret.M[1][0] = mat.b1;
    Ret.M[2][0] = mat.c1;
    Ret.M[3][0] = mat.d1;

    Ret.M[0][1] = mat.a2;
    Ret.M[1][1] = mat.b2;
    Ret.M[2][1] = mat.c2;
    Ret.M[3][1] = mat.d2;

    Ret.M[0][2] = mat.a3;
    Ret.M[1][2] = mat.b3;
    Ret.M[2][2] = mat.c3;
    Ret.M[3][2] = mat.d3;

    Ret.M[0][3] = mat.a4;
    Ret.M[1][3] = mat.b4;
    Ret.M[2][3] = mat.c4;
    Ret.M[3][3] = mat.d4;*/

    //Ret.M[0][0] = mat[0][0];
    //Ret.M[0][1] = mat[0][1];
    //Ret.M[0][2] = mat[0][2];
    //Ret.M[0][3] = mat[0][3];

    //Ret.M[1][0] = mat[1][0];
    //Ret.M[1][1] = mat[1][1];
    //Ret.M[1][2] = mat[1][2];
    //Ret.M[1][3] = mat[1][3];

    //Ret.M[2][0] = mat[2][0];
    //Ret.M[2][1] = mat[2][1];
    //Ret.M[2][2] = mat[2][2];
    //Ret.M[2][3] = mat[2][3];

    //Ret.M[3][0] = mat[3][0];
    //Ret.M[3][1] = mat[3][1];
    //Ret.M[3][2] = mat[3][2];
    //Ret.M[3][3] = mat[3][3];

    return Ret;
}

RailwaysNode::RailwaysNode(const FString& FilePath, TMap<FString, uint32>& m_BoneMapping, TArray<RailwaysBone>& Bones, const aiScene* scene, aiNode* node, const FTransform& ParentTransform)
{

    Name = UTF8_TO_TCHAR(node->mName.C_Str());

    LocalTransformMatrix = AIMattoFMatrix(node->mTransformation);
    Transform = FTransform(AIMattoFMatrix(node->mTransformation)) * ParentTransform; //ParentTransform * 
    //Transform = ParentTransform * FTransform(AIMattoFMatrix(node->mTransformation)); //ParentTransform * 
    //Transform.ScaleTranslation(FVector(100.0f));
    FVector Location = Transform.GetLocation();
    FQuat Rotation = Transform.GetRotation();
    FVector Scale = Transform.GetScale3D();
    UE_LOG(LogTemp, Warning, TEXT("Node: %s"), *Name);

    Meshes.AddDefaulted(node->mNumMeshes);
    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        //RailwaysMesh* NewMesh = new RailwaysMesh();
        RailwaysMesh& NewMesh = Meshes[i];
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        NewMesh.Name = UTF8_TO_TCHAR(mesh->mName.C_Str());

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
            //const FVector scalefactor(1.0f, -1.0f, -1.0f);
            const FVector scalefactor(1.0f, 1.0f, 1.0f);

            //scalefactor *= FVector(10.0f);
            //NewMesh->Vertices.Emplace(AIVEC3_TO_FVEC(Vert) * scalefactor, AIVEC3_TO_FVEC(Normal) * scalefactor, AIVEC3_TO_FVEC(Tangent) * scalefactor, TexCoords);

            const bool PreTransform = true;
            if (PreTransform)
            {
                NewMesh.Vertices.Emplace(Transform.TransformFVector4(AIVEC3_TO_FVEC(Vert)),
                    Transform.TransformVector(AIVEC3_TO_FVEC(Normal)),
                    Transform.TransformVector(AIVEC3_TO_FVEC(Tangent)),
                    TexCoords);
            }
            else
            {
                NewMesh.Vertices.Emplace(AIVEC3_TO_FVEC(Vert),
                    AIVEC3_TO_FVEC(Normal),
                    AIVEC3_TO_FVEC(Tangent),
                    TexCoords);
            }
        }

        for (uint32 t = 0; t < mesh->mNumFaces; t++)
        {
            const aiFace* face = &mesh->mFaces[t];
            for (unsigned int j = 0; j < face->mNumIndices; j++)
                NewMesh.Elements.Add(face->mIndices[j]);
        }

        //aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];
        NewMesh.MaterialIndex = mesh->mMaterialIndex;

        for (uint32 t = 0; t < mesh->mNumBones; t++)
        {
            const aiBone* bone = mesh->mBones[t];
            FString BoneName = UTF8_TO_TCHAR(bone->mName.C_Str());
            uint32 BoneIndex = 0;
            if (m_BoneMapping.Find(BoneName) == nullptr)
            {
                //create the new bone
                BoneIndex = Bones.Num();
                Bones.Add(RailwaysBone());
                m_BoneMapping.Add(BoneName, BoneIndex);
                Bones[BoneIndex].Name = BoneName;
                Bones[BoneIndex].FName = FName(*BoneName);
            }
            else
            {
                BoneIndex = m_BoneMapping[BoneName];
            }


            RailwaysBone& NewBone = Bones[BoneIndex];

            //NewBone.Name = BoneName;
            //NewBone.FName = FName(*BoneName);
            //it's the inverse, look into getting this to be local?
            NewBone.OffsetTransform = Transform.ToMatrixWithScale() * AIMattoFMatrix(bone->mOffsetMatrix); //might need to add the node transform here  * Transform FTransform(AIMattoFMatrix(bone->mOffsetMatrix))
            //NewBone.Transform = Transform; //might need to add the node transform here  * Transform FTransform(AIMattoFMatrix(bone->mOffsetMatrix))
            UE_LOG(LogTemp, Display, TEXT("Loaded bone %s with %i weights"), *NewBone.Name, bone->mNumWeights);


            for (uint32 w = 0; w < bone->mNumWeights; w++)
            {
                const aiVertexWeight& weight = bone->mWeights[w];
                RailwaysVert& TargetVertex = NewMesh.Vertices[weight.mVertexId];

                TargetVertex.InfluenceBones.Add(BoneIndex);
                TargetVertex.InfluenceWeights.Add((uint8)(255.0f * weight.mWeight));

                //NewMesh->Vertices[weight.mVertexId].InfluenceWeights[] = weight.mWeight;
            }

        }

        UE_LOG(LogTemp, Display, TEXT("Loaded mesh %s with %i verts and %i triangles"), *NewMesh.Name, NewMesh.Vertices.Num(), NewMesh.Elements.Num() / 3);
        //Meshes.Add(NewMesh);
    }

    bool IsEmptyBone = false;
    uint32 BoneIndex = 0;
    if (Name.Contains(TEXT("b.r.")))
    {
        FString BoneName = Name;
        if (!m_BoneMapping.Contains(BoneName))
        {
            IsEmptyBone = true;
            //create the new bone
            BoneIndex = Bones.Num();
            Bones.Add(RailwaysBone());
            m_BoneMapping.Add(BoneName, BoneIndex);
            RailwaysBone& NewBone = Bones[BoneIndex];

            NewBone.Name = BoneName;
            NewBone.FName = FName(*BoneName);
            //it's the inverse, look into getting this to be local?
            NewBone.OffsetTransform = Transform.ToMatrixWithScale(); //might need to add the node transform here  * Transform FTransform(AIMattoFMatrix(bone->mOffsetMatrix))
            //NewBone.Transform = Transform; //might need to add the node transform here  * Transform FTransform(AIMattoFMatrix(bone->mOffsetMatrix))
            UE_LOG(LogTemp, Display, TEXT("Loaded bone %s"), *NewBone.Name);

        }
    }

    for (uint32 i = 0; i < node->mNumChildren; i++)
        Children.Emplace(FilePath, m_BoneMapping, Bones, scene, node->mChildren[i], Transform); //aiTransform

    if (IsEmptyBone && BoneIndex != 0) RecursiveParentEmpty(BoneIndex);

    //ProcessNode(FilePath, scene, node->mChildren[i], NewNode);
}

void RailwaysNode::RecursiveParentEmpty(int32 ParentIdx)
{

    for (auto& mesh : Meshes)
    {
        for (auto& vert : mesh.Vertices)
        {
            //vert.InfluenceBones.Empty();
            //vert.InfluenceWeights.Empty();
            //vert.InfluenceBones.Add(ParentIdx);
            //vert.InfluenceWeights.Add((uint8)(255.0f));
            if (!vert.InfluenceBones.Num()) vert.InfluenceBones.AddUninitialized(1);
            if (!vert.InfluenceWeights.Num()) vert.InfluenceWeights.AddUninitialized(1);

            vert.InfluenceBones[0] = ParentIdx;
            vert.InfluenceWeights[0] = 255;
        }
    }
    for (auto& child : Children)
        child.RecursiveParentEmpty(ParentIdx);
}

void RailwaysNode::ProcessBoneHierarchy(TMap<FString, uint32>& m_BoneMapping, TArray<RailwaysBone>& Bones, const FMatrix& ParentTransform, int ParentIndex)
{
    //FTransform BoneTransform = FTransform(Transform) * ParentTransform;
    //FMatrix BoneTransform = ParentTransform * LocalTransformMatrix;
    int32 BoneIndex = INDEX_NONE;
    if (m_BoneMapping.Find(Name))
    {
        //we know that this node is responsible for a bone
        BoneIndex = m_BoneMapping[Name];
        RailwaysBone& Bone = Bones[BoneIndex];
        Bone.Transform = FTransform(ParentTransform * LocalTransformMatrix);
        //Bone.Transform = Transform;

        Bone.ParentIndex = ParentIndex;
        UE_LOG(LogTemp, Warning, TEXT("parent for bone %s is %i"), *Name, ParentIndex);
        //BoneTransform = BoneTransform * Bone.OffsetTransform;
    }
    /*for (auto& child : Children)
        child.ProcessBoneHierarchy(m_BoneMapping, Bones, Transform, BoneIndex);*/
    for (auto& child : Children)
        child.ProcessBoneHierarchy(m_BoneMapping, Bones, ParentTransform, BoneIndex); //BoneTransform
}

bool RailwaysNode::Serialize(FArchive& Ar)
{
    Ar << Name;
    Ar << Transform;

    int32 MeshCount = Meshes.Num();
    Ar << MeshCount;

    if (Ar.IsLoading()) Meshes.Reset(MeshCount);
    for (int32 i = 0; i < MeshCount; i++)
    {
        RailwaysMesh& Mesh = Meshes[i];
        //Ar << Mesh.Vertices;
        int32 vertCount = Mesh.Vertices.Num();
        Ar << vertCount;
        if (Ar.IsLoading()) Mesh.Vertices.AddUninitialized(vertCount);
        for (int32 v = 0; v < vertCount; v++)
        {
            RailwaysVert& Vert = Mesh.Vertices[v];
            Ar << Vert.Location;
            Ar << Vert.Normal;
            Ar << Vert.Tangent;
            Ar << Vert.TexCoords;
            Ar << Vert.InfluenceBones;
            Ar << Vert.InfluenceWeights;
        }
        Ar << Mesh.Elements;

        //material index
    }

    int32 ChildCount = Children.Num();
    Ar << ChildCount;
    if (Ar.IsLoading()) Children.Reset(ChildCount);
    for (int32 i = 0; i < ChildCount; i++)
        Children[i].Serialize(Ar);

    return true;
}

bool FMeshContent::Serialize(FArchive& Ar)
{
    if (Ar.IsSaving())
    {
        Ar.Serialize("RMSH", 4);
    }
    else
    {
        char Header[4];
        Ar.Serialize(&Header[0], 4);
        if (!strcmp("RMSH", &Header[0]))
        {
            UE_LOG(LogTemp, Error, TEXT("Incorrect mesh header!"));
            return false;
        }
    }

    int32 Version = 1;
    Ar << Version;

    //bounds
    //static or skeletal type

    MeshData->RootNode->Serialize(Ar);

    return true;
}

void FMeshContent::SaveMesh(FString Path)
{
    TArray<uint8> Buffer;

    FMemoryWriter Writer(Buffer);
    Serialize(Writer);
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    auto file = PlatformFile.OpenWrite(TEXT("E:/Users/riley_000/Documents/Unreal Projects/Railways/Plugins/ContentSystem/Content/Samples/test2.rmsh"));
    file->Write(Buffer.GetData(), Buffer.Num());
    //file->Flush();

    delete file; //close handle
}

void FMeshContent::LoadMesh(FString Path)
{

}

FMeshContent::FMeshContent()
{
}

FMeshContent::~FMeshContent()
{
}
