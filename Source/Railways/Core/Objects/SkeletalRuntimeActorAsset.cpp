// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalRuntimeActorAsset.h"
//#include "RenderUtils.h"
//#include "ImageUtils.h"
#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkinWeightVertexBuffer.h"
#include "../RailwaysGameInstance.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Helpers/stb_image.h"
#include "Developer/MeshUtilities/Private/SkeletalMeshTools.h"

ASkeletalRuntimeActorAsset::ASkeletalRuntimeActorAsset()
{
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

}

ASkeletalRuntimeActorAsset::~ASkeletalRuntimeActorAsset()
{
    if (SkeletalMesh)
    {
        SkeletalMesh->ReleaseResources();
    }
}

void ASkeletalRuntimeActorAsset::BeginPlay()
{
    Super::BeginPlay();

    MeshComponent->SetRenderStatic(true);
    Skeleton = NewObject<USkeleton>(this, TEXT("Skeleton"));
    SkeletalMesh = NewObject<USkeletalMesh>(this, TEXT("Mesh"));

    //NewTile->RegisterComponent();

    URailwaysGameInstance* GameInstance = Cast<URailwaysGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        MeshData = GameInstance->AssimpInterface->ImportFBX(false);
        InitAsset();
    }
}

// Called every frame
void ASkeletalRuntimeActorAsset::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ASkeletalRuntimeActorAsset::InitAsset()
{
    FSkeletalMeshLODRenderData* LOD = new FSkeletalMeshLODRenderData();
    LOD->RenderSections.Empty();
    //LOD0.IndexBuffer.Empty();
    
    FLocalVertexFactory* VertexFactory = new FLocalVertexFactory(ERHIFeatureLevel::SM5, "FProcSkeletalMeshSection");
    TArray<FDynamicMeshVertex> Vertices;
    TArray<FSoftSkinVertex> SkinnedVertices;
    TArray<uint32> Elements;

    FSkeletalMeshLODInfo LODInfo;
    LODInfo.ScreenSize = 1.0f;

    GenerateSkeletalRenderMesh(Vertices, SkinnedVertices, Elements, LOD, LODInfo, *MeshData->RootNode);
    SkeletalMesh->AddLODInfo(LODInfo);

    //	//	VertexBuffers.InitWithDummyData(&VertexFactory, NumVertices);
//	//	IndexBuffer.NumIndices = NumIndices;
//	//	BeginInitResource(&IndexBuffer);
    LOD->StaticVertexBuffers.InitWithDummyData(VertexFactory, Vertices.Num());
    LOD->StaticVertexBuffers.InitFromDynamicVertex(VertexFactory, Vertices, 1);

    //LOD->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint32));
    if (!LOD->MultiSizeIndexContainer.IsIndexBufferValid()) LOD->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint32));
    LOD->MultiSizeIndexContainer.CopyIndexBuffer(Elements);
    BeginInitResource(LOD->MultiSizeIndexContainer.GetIndexBuffer());
    //LOD->MultiSizeIndexContainer.InitResources();

    /*for (const uint32& element : Mesh->Elements)
        LOD->MultiSizeIndexContainer.GetIndexBuffer()->AddItem(element);*/

    //skinning init

    //Copy to weight info
    TArray<FSkinWeightInfo> VertexWeightInfos;
    VertexWeightInfos.AddUninitialized(SkinnedVertices.Num());

    for (int32 VertIdx = 0; VertIdx < SkinnedVertices.Num(); VertIdx++)
    {
        const FSoftSkinVertex& SrcVertex = SkinnedVertices[VertIdx];
        FSkinWeightInfo& DstVertex = VertexWeightInfos[VertIdx];
        FMemory::Memcpy(DstVertex.InfluenceBones, SrcVertex.InfluenceBones, MAX_TOTAL_INFLUENCES * sizeof(FBoneIndexType));
        FMemory::Memcpy(DstVertex.InfluenceWeights, SrcVertex.InfluenceWeights, MAX_TOTAL_INFLUENCES * sizeof(uint8));

    }
    uint32 TotalNumUsedBones = 0;
    bool bVariableBonesPerVertex = LOD->SkinWeightVertexBuffer.GetVariableBonesPerVertex();

    //Get buffers
    FSkinWeightLookupVertexBuffer& LookupVertexBuffer = *const_cast<FSkinWeightLookupVertexBuffer*>(LOD->SkinWeightVertexBuffer.GetLookupVertexBuffer());
    FSkinWeightDataVertexBuffer& DataVertexBuffer = *const_cast<FSkinWeightDataVertexBuffer*>(LOD->SkinWeightVertexBuffer.GetDataVertexBuffer());

    

    //init resources
    SkeletalMesh->AllocateResourceForRendering();

    ENQUEUE_RENDER_COMMAND(InitProceduralSkeletalMesh)(
        [this, LOD](FRHICommandListImmediate& RHICmdList)
        {
            FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
            RenderData->LODRenderData.Add(LOD);
            //SkeletalMesh->InitResources();
        });

    MeshComponent->SetSkeletalMesh(SkeletalMesh);

    //should happen in render thread?
    //SkeletalMesh->InitResources();

    //BuildMeshNode(LOD0, *MeshData->RootNode);
}

//struct FSoftSkinBuildVertex
//{
//    FVector			Position;
//    FVector			TangentX,	// Tangent, U-direction
//        TangentY,	// Binormal, V-direction
//        TangentZ;	// Normal
//    FVector2D		UVs[MAX_TEXCOORDS]; // UVs
//    FColor			Color;		// VertexColor
//    FBoneIndexType	InfluenceBones[MAX_TOTAL_INFLUENCES];
//    uint8			InfluenceWeights[MAX_TOTAL_INFLUENCES];
//    uint32 PointWedgeIdx;
//};

void ASkeletalRuntimeActorAsset::GenerateSkeletalRenderMesh(TArray<FDynamicMeshVertex>& Vertices, TArray<FSoftSkinVertex>& SkinnedVertices, TArray<uint32>& Elements, FSkeletalMeshLODRenderData* LOD, FSkeletalMeshLODInfo& LODInfo, const AssimpNode& Node)
{
    const int32 NumMeshes = Node.Meshes.Num();
    static int MeshIndex = 0;
    for (const AssimpMesh* Mesh : Node.Meshes)
    {
        FSkelMeshRenderSection& Section = *new(LOD->RenderSections) FSkelMeshRenderSection();
        Section.MaterialIndex = MeshIndex;
        Section.BaseVertexIndex = Vertices.Num();
        Section.BaseIndex = Elements.Num();
        //Generate
        //FLocalVertexFactory VertexFactory(GetScene().GetFeatureLevel());
        Section.DuplicatedVerticesBuffer.bHasOverlappingVertices = false;
        Section.DuplicatedVerticesBuffer.DupVertData.Add(0);
        Section.DuplicatedVerticesBuffer.DupVertIndexData.Add(FIndexLengthPair());

        for (const auto& Vert : Mesh->Vertices)
        {
            //static
            FDynamicMeshVertex Vertex;
            Vertex.Position = Vert.Location;
            Vertex.Color = FColor::White;
            Vertex.TangentX = Vert.Tangent;
            //Vertex.TangentY = FVector::CrossProduct(Vert.Tangent, Vert.Normal);
            Vertex.TangentZ = Vert.Normal;
            Vertex.TextureCoordinate[0] = Vert.TexCoords;
            Vertices.Add(Vertex);

            //skinned
            FSoftSkinVertex SkinnedVertex;
            SkinnedVertex.Position = Vert.Location;
            SkinnedVertex.Color = FColor::White;
            SkinnedVertex.TangentX = Vert.Tangent;
            SkinnedVertex.TangentY = FVector::CrossProduct(Vert.Tangent, Vert.Normal);
            SkinnedVertex.TangentZ = Vert.Normal;
            SkinnedVertex.UVs[0] = Vert.TexCoords;

            //Bone Defaults
            SkinnedVertex.InfluenceBones[0] = 0;
            SkinnedVertex.InfluenceWeights[0] = 255;
            for (uint32 i = 1; i < MAX_TOTAL_INFLUENCES; i++)
            {
                SkinnedVertex.InfluenceBones[i] = 0;
                SkinnedVertex.InfluenceWeights[i] = 0;
            }

            for (int32 i = 0; i < FMath::Min(Vert.InfluenceBones.Num(), MAX_TOTAL_INFLUENCES); i++)
            {
                SkinnedVertex.InfluenceBones[i] = Vert.InfluenceBones[i];
                SkinnedVertex.InfluenceWeights[i] = Vert.InfluenceWeights[i];

                //Add to bone map
                Section.BoneMap.AddUnique(SkinnedVertex.InfluenceBones[i]);
                LOD->ActiveBoneIndices.AddUnique(SkinnedVertex.InfluenceBones[i]);
            }
            SkinnedVertices.Add(SkinnedVertex);

        }

        for (const uint32& element : Mesh->Elements)
            Elements.Add(Section.BaseVertexIndex + element);

        //if (bVariableBonesPerVertex)
        //{
        //    LookupVertexBuffer.Init(VertexWeightInfos.Num());
        //    for (int32 VertIdx = 0; VertIdx < VertexWeightInfos.Num(); VertIdx++)
        //    {
        //        int32 NumUsedBones = 0;
        //        for (int32 InfluenceIdx = 0; InfluenceIdx < MAX_TOTAL_INFLUENCES; InfluenceIdx++)
        //        {
        //            if (VertexWeightInfos[VertIdx].InfluenceWeights[InfluenceIdx] > 0)
        //            {
        //                NumUsedBones++;
        //            }
        //        }

        //        LookupVertexBuffer.SetWeightOffsetAndInfluenceCount(VertIdx, TotalNumUsedBones * (LOD->SkinWeightVertexBuffer.GetBoneIndexByteSize() + sizeof(uint8)), NumUsedBones);
        //        TotalNumUsedBones += NumUsedBones;
        //    }
        //}
        //else
        //{
        //    TotalNumUsedBones = LOD->SkinWeightVertexBuffer.GetMaxBoneInfluences() * VertexWeightInfos.Num();
        //}

        //DataVertexBuffer.Init(TotalNumUsedBones, VertexWeightInfos.Num());
        //for (int32 VertIdx = 0; VertIdx < VertexWeightInfos.Num(); VertIdx++)
        //{
        //    if (bVariableBonesPerVertex)
        //    {
        //        // Copy the weights and sort in descending order
        //        FSkinWeightInfo VertexWeights = VertexWeightInfos[VertIdx];
        //        for (uint32 i = 0; i < MAX_TOTAL_INFLUENCES; i++)
        //        {
        //            uint32 MaxWeightIdx = i;
        //            for (uint32 j = i + 1; j < MAX_TOTAL_INFLUENCES; j++)
        //            {
        //                if (VertexWeights.InfluenceWeights[j] > VertexWeights.InfluenceWeights[MaxWeightIdx])
        //                {
        //                    MaxWeightIdx = j;
        //                }
        //            }
        //            if (MaxWeightIdx != i)
        //            {
        //                Exchange(VertexWeights.InfluenceBones[i], VertexWeights.InfluenceBones[MaxWeightIdx]);
        //                Exchange(VertexWeights.InfluenceWeights[i], VertexWeights.InfluenceWeights[MaxWeightIdx]);
        //            }
        //        }

        //        int32 NumUsedBones = 0;
        //        for (uint32 InfluenceIdx = 0; InfluenceIdx < LOD->SkinWeightVertexBuffer.GetMaxBoneInfluences(); InfluenceIdx++)
        //        {
        //            if (VertexWeights.InfluenceWeights[InfluenceIdx] > 0)
        //            {
        //                LOD->SkinWeightVertexBuffer.SetBoneIndex(VertIdx, NumUsedBones, VertexWeights.InfluenceBones[InfluenceIdx]);
        //                LOD->SkinWeightVertexBuffer.SetBoneWeight(VertIdx, NumUsedBones, VertexWeights.InfluenceWeights[InfluenceIdx]);
        //                NumUsedBones++;
        //            }
        //        }
        //    }
        //    else
        //    {
        //        const FSkinWeightInfo& VertexWeights = VertexWeightInfos[VertIdx];
        //        for (uint32 InfluenceIdx = 0; InfluenceIdx < LOD->SkinWeightVertexBuffer.GetMaxBoneInfluences(); InfluenceIdx++)
        //        {
        //            LOD->SkinWeightVertexBuffer.SetBoneIndex(VertIdx, InfluenceIdx, VertexWeights.InfluenceBones[InfluenceIdx]);
        //            LOD->SkinWeightVertexBuffer.SetBoneWeight(VertIdx, InfluenceIdx, VertexWeights.InfluenceWeights[InfluenceIdx]);
        //        }
        //    }
        //}
        LODInfo.LODMaterialMap.Add(MeshIndex);
        SkeletalMesh->Materials.Add(UMaterial::GetDefaultMaterial(MD_Surface));
        //NewSection->Material = Component->GetMaterial(SectionIdx);
        //if (NewSection->Material == NULL)
        //{
        //	NewSection->Material = UMaterial::GetDefaultMaterial(MD_Surface);
        //}

        //LOD->StaticVertexBuffers.PositionVertexBuffer.Init(Positions);
        //LOD->StaticVertexBuffers.StaticMeshVertexBuffer.Init();

        //Build each mesh as a chunk?


        MeshIndex++;
    }

    for (auto& Child : Node.Children)
        GenerateSkeletalRenderMesh(Vertices, SkinnedVertices, Elements, LOD, LODInfo, Child);
}

//void ASkeletalRuntimeActorAsset::BuildMeshNode(FSkeletalMeshLODRenderData& LOD, const AssimpNode& Node)
//{
//    const int32 NumMeshes = Node.Meshes.Num();
//    static int MeshIndex = 0;
//    for (const AssimpMesh* Mesh : Node.Meshes)
//    {
//        FSkelMeshRenderSection NewSection;
//        NewSection.MaterialIndex = MeshIndex;
//
//        //const AssimpMesh* Mesh = Node.Meshes[i];
//        TArray<FVector> Positions;
//        TArray<FVector> Normals;
//        TArray<FVector2D> UVs;
//        TArray<FColor> Colors;
//        TArray<FProcMeshTangent> Tangents;
//
//        Positions.AddUninitialized(Mesh->Vertices.Num());
//        Normals.AddUninitialized(Mesh->Vertices.Num());
//        UVs.AddUninitialized(Mesh->Vertices.Num());
//        Colors.AddUninitialized(Mesh->Vertices.Num());
//        Tangents.AddUninitialized(Mesh->Vertices.Num());
//
//        for (int vert = 0; vert < Mesh->Vertices.Num(); vert++)
//        {
//            const AssimpVert& VertData = Mesh->Vertices[vert];
//            Positions[vert] = VertData.Location * 100.0f;
//            //Positions[vert] = VertData.Location * 10.0f;
//            //Positions[vert] = VertData.Location;
//            Normals[vert] = VertData.Normal;
//            UVs[vert] = VertData.TexCoords;
//            Colors[vert] = FColor(255, 255, 255);
//            Tangents[vert] = FProcMeshTangent(VertData.Tangent, false);
//        }
//
//        TArray<int32> Triangles;
//        Triangles.AddUninitialized(Mesh->Elements.Num());
//        for (int tri = 0; tri < Mesh->Elements.Num(); tri++)
//        {
//            Triangles[tri] = Mesh->Elements[tri];
//        }
//
//        //create material
//        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PBRMaterial, this);
//
//        //create textures
//        for (const auto& Texture : Mesh->Material.Textures)
//        {
//            int width, height, nrComponents;
//            unsigned char* data = stbi_load(TCHAR_TO_UTF8(*Texture.Path), &width, &height, &nrComponents, 4);
//            if (data)
//            {
//                FName TextureName = MakeUniqueObjectName(this, UTexture2D::StaticClass());
//                UTexture2D* NewTexture = NewObject<UTexture2D>(this, TextureName, RF_Transient);
//
//                EPixelFormat format = PF_R8G8B8A8;
//                NewTexture->PlatformData = new FTexturePlatformData();
//                NewTexture->PlatformData->SizeX = width;
//                NewTexture->PlatformData->SizeY = height;
//                NewTexture->PlatformData->PixelFormat = format;
//                if (Texture.Type == TextureType::Normal || Texture.Type == TextureType::Parameter)
//                {
//                    NewTexture->SRGB = false;
//                }
//                //mipmaps
//                int32 NumBlocksX = width;
//                int32 NumBlocksY = height;
//                FTexture2DMipMap* Mip = new(NewTexture->PlatformData->Mips) FTexture2DMipMap();
//                Mip->SizeX = width;
//                Mip->SizeY = height;
//                Mip->BulkData.Lock(LOCK_READ_WRITE);
//                void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * 4); //GPixelFormats[format].BlockBytes
//                FMemory::Memcpy(TextureData, data, width * height * 4);
//                Mip->BulkData.Unlock();
//
//                NewTexture->UpdateResource();
//
//                stbi_image_free(data);
//
//                FName TextureTarget;
//                switch (Texture.Type)
//                {
//                default:
//                case TextureType::Diffuse:
//                    TextureTarget = FName(TEXT("BaseColor"));
//                    break;
//                case TextureType::Normal:
//                    TextureTarget = FName(TEXT("Normal"));
//                    break;
//                case TextureType::Parameter:
//                    TextureTarget = FName(TEXT("Parameter"));
//                    break;
//                }
//
//                DynMaterial->SetTextureParameterValue(TextureTarget, NewTexture);
//            }
//        }
//
//        //MeshComponent->SetMaterial(MeshIndex, DynMaterial);
//
//        //MeshComponent->CreateMeshSection(MeshIndex, Positions, Triangles, Normals, UVs, Colors, Tangents, false);
//        MeshIndex++;
//    }
//
//    for (const AssimpNode& Child : Node.Children)
//        BuildMeshNode(LOD, Child);
//}

