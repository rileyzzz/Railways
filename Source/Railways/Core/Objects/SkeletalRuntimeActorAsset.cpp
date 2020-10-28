// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalRuntimeActorAsset.h"
//#include "RenderUtils.h"
//#include "ImageUtils.h"
#include "Engine/SkeletalMesh.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Rendering/SkinWeightVertexBuffer.h"
#include "Animation/AnimCompress.h"
//#include "Animation/AnimCompressionDerivedData.h"
#include "../RailwaysGameInstance.h"
#include "DrawDebugHelpers.h"
//#include "Developer/MeshUtilities/Private/SkeletalMeshTools.h"

//compiler gets mad if i don't have this for some reason
#if !WITH_EDITOR
struct FSoftSkinVertex
{
    FVector			Position;

    // Tangent, U-direction
    FVector			TangentX;
    // Binormal, V-direction
    FVector			TangentY;
    // Normal
    FVector4		TangentZ;

    // UVs
    FVector2D		UVs[MAX_TEXCOORDS];
    // VertexColor
    FColor			Color;
    FBoneIndexType	InfluenceBones[MAX_TOTAL_INFLUENCES];
    uint8			InfluenceWeights[MAX_TOTAL_INFLUENCES];

};
#endif

ASkeletalRuntimeActorAsset::ASkeletalRuntimeActorAsset()
{
    //MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent2"));
    //MeshComponent->SetupAttachment(RootComponent);

}

ASkeletalRuntimeActorAsset::~ASkeletalRuntimeActorAsset()
{
    for (const auto& SkeletalMesh : SkeletalMeshes)
    {
        if (SkeletalMesh)
            SkeletalMesh->ReleaseResources();
    }
}

void ASkeletalRuntimeActorAsset::BeginPlay()
{
    Super::BeginPlay();

    MeshComponent = NewObject<USkeletalMeshComponent>(this, TEXT("MeshComponent2"));
    //MeshComponent = NewObject<UPoseableMeshComponent>(this, TEXT("MeshComponent2"));
    MeshComponent->RegisterComponent();
    MeshComponent->AttachTo(RootComponent);
    //MeshComponent->SetRenderStatic(false);
    //Skeleton = NewObject<USkeleton>(this, TEXT("Skeleton"));
    //SkeletalMesh = NewObject<USkeletalMesh>(this, TEXT("Mesh2"));

    //NewTile->RegisterComponent();

    InitAsset();
}

// Called every frame
void ASkeletalRuntimeActorAsset::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //static int32 frame = 0;

    //for (int32 i = 0; i < MeshData->Bones.Num(); i++)
    //{
    //    const auto& Bone = MeshData->Bones[i];
    //    if (Bone.AnimTracks.Num())
    //    {
    //        const auto& track = Bone.AnimTracks[0];
    //        if (frame >= track.PosFrames.Num()) frame = 0;

    //        auto TargetBone = MeshComponent->GetBoneName(i);
    //        //FRotator BoneRotation = MeshComponent->GetBoneRotationByName(targetbone, EBoneSpaces::ComponentSpace);
    //        //BoneRotation.Add(0.0f, 0.0f, 0.5f);
    //        MeshComponent->SetBoneScaleByName(TargetBone, track.ScaleFrames[FMath::Min(frame, track.ScaleFrames.Num() - 1)], EBoneSpaces::WorldSpace);
    //        MeshComponent->SetBoneRotationByName(TargetBone, FRotator(track.RotFrames[FMath::Min(frame, track.RotFrames.Num() - 1)]), EBoneSpaces::WorldSpace);
    //        MeshComponent->SetBoneLocationByName(TargetBone, track.PosFrames[FMath::Min(frame, track.PosFrames.Num() - 1)], EBoneSpaces::WorldSpace);
    //    }
    //}
    /*auto targetbone = MeshComponent->GetBoneName(2);
    FRotator BoneRotation = MeshComponent->GetBoneRotationByName(targetbone, EBoneSpaces::ComponentSpace);
    BoneRotation.Add(0.0f, 0.0f, 0.5f);
    MeshComponent->SetBoneRotationByName(targetbone, BoneRotation, EBoneSpaces::ComponentSpace);*/
    //frame++;
}



void ASkeletalRuntimeActorAsset::InitAsset()
{
    Super::InitAsset(); //creates material instances
    //FSuspendRenderingThread suspend(false);
    //MeshComponent->SetRelativeScale3D(FVector(30.0f));

    for (auto& Part : MeshParts)
        SkeletalMeshes.Add(BuildMesh(Part));
}

USkeletalMesh* ASkeletalRuntimeActorAsset::BuildMesh(FMeshPart& Part)
{
    USkeletalMesh* SkeletalMesh = NewObject<USkeletalMesh>(this, NAME_Components);

#if !USE_EDITOR_RENDER
    SkeletalMesh->ReleaseResources();
#endif

#if USE_EDITOR_RENDER
    SkeletalMesh->EmptyAllImportData();
    SkeletalMesh->GetImportedModel()->LODModels.Empty();
    FSkeletalMeshLODModel* LOD = new FSkeletalMeshLODModel();
    SkeletalMesh->GetImportedModel()->LODModels.Add(LOD);
    LOD->Sections.Empty();
#else
    FSkeletalMeshLODRenderData* LOD = new FSkeletalMeshLODRenderData();
    SkeletalMesh->AllocateResourceForRendering();
    SkeletalMesh->GetResourceForRendering()->LODRenderData.Add(LOD);
#endif

    TArray<FSoftSkinVertex> Vertices;
    TArray<uint32> Elements;

    FSkeletalMeshLODInfo LODInfo;
    LODInfo.ScreenSize = 1.0f;
    LODInfo.LODHysteresis = 0.2f;

#if USE_EDITOR_RENDER
    GenerateSkeletalRenderMesh(Vertices, Elements, LOD, LODInfo, Part.MeshContent.MeshData->RootNode, Part.MeshData->Bones.Num());
#else
    GenerateSkeletalRenderMesh(Vertices, Elements, LOD, LODInfo, Part.MeshContent.MeshData->RootNode, Part.MeshContent.MeshData->Bones.Num());
#endif

    //material setup
    for (int32 i = 0; i < Part.MeshContent.MeshData->Materials.Num(); i++)
    {
        //default material
        SkeletalMesh->Materials.Add(UMaterial::GetDefaultMaterial(MD_Surface));
        SkeletalMesh->Materials.Last().UVChannelData.bInitialized = true;
    }
    //for (UMaterialInstanceDynamic* Instance : MaterialInstances)
    //{
    //    SkeletalMesh->Materials.Add(Instance);
    //    SkeletalMesh->Materials.Last().UVChannelData.bInitialized = true;
    //}

    //bone setup
    for (int i = 0; i < Part.MeshContent.MeshData->Bones.Num(); i++)
    {
        const auto& Bone = Part.MeshContent.MeshData->Bones[i];

        LOD->RequiredBones.Add(i);
        LOD->ActiveBoneIndices.Add(i);

        //FName BoneName = Bone.FName;

        FTransform Transform = Bone.Transform;

        //int32 ParentIndex = INDEX_NONE; //used to determine root bone
        //if (i > 0) ParentIndex = Bone.ParentIndex;
        int32 ParentIndex = Bone.ParentIndex;
        if (i > 0 && ParentIndex == INDEX_NONE) ParentIndex = 0;

        FVector Location = Transform.GetLocation();
        UE_LOG(LogTemp, Warning, TEXT("Bone %s location %f %f %f"), *Bone.Name, Location.X, Location.Y, Location.Z);
        //FTransform WorldTransform = GetActorTransform() * Transform;
        //DrawDebugLine(GetWorld(), WorldTransform.GetLocation(), WorldTransform.GetLocation() + Transform.TransformPosition(FVector(0.0f, 0.0f, 1.0f)), FColor::Green, true);

        FReferenceSkeletonModifier Modifier = FReferenceSkeletonModifier(SkeletalMesh->RefSkeleton, nullptr);
        Modifier.Add(FMeshBoneInfo(Bone.FName, Bone.Name, ParentIndex), Bone.Transform);
    }


    FBox BoundBox = FBox();
    BoundBox.Init();

#if !USE_EDITOR_RENDER
    LOD->StaticVertexBuffers.PositionVertexBuffer.Init(Vertices.Num());
    LOD->StaticVertexBuffers.ColorVertexBuffer.Init(Vertices.Num());
    LOD->StaticVertexBuffers.StaticMeshVertexBuffer.Init(Vertices.Num(), 1);

    TArray<FSkinWeightInfo> VertexWeightInfos;
    VertexWeightInfos.AddUninitialized(Vertices.Num());
#endif

    for (int32 VertIndex = 0; VertIndex < Vertices.Num(); VertIndex++)
    {
        const auto& Vert = Vertices[VertIndex];
        BoundBox += Vert.Position;
#if !USE_EDITOR_RENDER
        LOD->StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertIndex) = Vert.Position;
        LOD->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(VertIndex, Vert.TangentX, Vert.TangentY, Vert.TangentZ);
        LOD->StaticVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(VertIndex, 0, Vert.UVs[0]);

        FSkinWeightInfo& DstInfo = VertexWeightInfos[VertIndex];

        FMemory::Memcpy(DstInfo.InfluenceBones, Vert.InfluenceBones, MAX_TOTAL_INFLUENCES * sizeof(FBoneIndexType));
        FMemory::Memcpy(DstInfo.InfluenceWeights, Vert.InfluenceWeights, MAX_TOTAL_INFLUENCES * sizeof(uint8));
#endif
    }

#if !USE_EDITOR_RENDER
    //LOD->SkinWeightVertexBuffer = VertexWeightInfos;
    LOD->StaticVertexBuffers.ColorVertexBuffer.InitFromSingleColor(FColor::White, Vertices.Num());
#endif


#if USE_EDITOR_RENDER
    LOD->NumVertices = Vertices.Num();
    LOD->NumTexCoords = 1;
#else
    LOD->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint32));
#endif
#if USE_EDITOR_RENDER 
    for (const auto& Element : Elements)
    {
        LOD->IndexBuffer.Add(Element);
        //#else
                //LOD->MultiSizeIndexContainer.GetIndexBuffer()->AddItem(Element);
}
#else
    LOD->MultiSizeIndexContainer.CopyIndexBuffer(Elements);
#endif


    //if (!LOD->MultiSizeIndexContainer.IsIndexBufferValid()) LOD->MultiSizeIndexContainer.CreateIndexBuffer(sizeof(uint32));
    //LOD->MultiSizeIndexContainer.CopyIndexBuffer(Elements);

    //BeginInitResource(LOD->MultiSizeIndexContainer.GetIndexBuffer());

    FBoxSphereBounds Bounds(BoundBox);
    Bounds = Bounds.ExpandBy(100000.0f);
    SkeletalMesh->SetImportedBounds(Bounds);

    SkeletalMesh->ResetLODInfo();
    SkeletalMesh->AddLODInfo(LODInfo);
    UE_LOG(LogTemp, Warning, TEXT("lod count %i"), SkeletalMesh->GetLODInfoArray().Num());
#if USE_EDITOR_RENDER
    //SkeletalMesh->PostEditChange();
    //SkeletalMesh->ReleaseResources();
    //SkeletalMesh->AllocateResourceForRendering();
    //SkeletalMesh->InitResources();

    //SkeletalMesh->Build();
    SkeletalMesh->PostEditChange();
#else
    //SkeletalMesh->AllocateResourceForRendering();
    //SkeletalMesh->GetResourceForRendering()->LODRenderData.Add(LOD);
    //SkeletalMesh->InitResources();
    SkeletalMesh->ReleaseResources();
    SkeletalMesh->InitResources();
#endif

    SkeletalMesh->Skeleton = NewObject<USkeleton>();
    SkeletalMesh->Skeleton->MergeAllBonesToBoneTree(SkeletalMesh);

    //#if !USE_EDITOR_RENDER && WITH_EDITOR
    //    //Add a dummy LOD so it doesn't complain
    //    FSkeletalMeshLODModel* DummyLOD = new FSkeletalMeshLODModel();
    //    SkeletalMesh->GetImportedModel()->LODModels.Add(DummyLOD);
    //#endif

#if WITH_EDITOR
    SkeletalMesh->CalculateInvRefMatrices();
    SkeletalMesh->CalculateExtendedBounds();
#else
    SkeletalMesh->PostLoad(); //will reset lod data in editor
#endif


    MeshComponent->SetSkeletalMesh(SkeletalMesh);


    //Animation = NewObject<UAnimSequence>(MeshComponent);
    ////Animation->AdditiveAnimType = AAT_LocalSpaceBase;
    ////Animation->RefPoseType = ABPT_RefPose;

    //Animation->SetSkeleton(SkeletalMesh->Skeleton); //important
    //Animation->SequenceLength = MeshData->AnimData[0].Duration;
    //int32 FrameCount = 0;

    //TArray<FRawAnimSequenceTrack>& Tracks = const_cast<TArray<FRawAnimSequenceTrack>&>(Animation->GetRawAnimationData());
    //TArray<FTrackToSkeletonMap>& TrackToSkeletonMap = const_cast<TArray<FTrackToSkeletonMap>&>(Animation->GetRawTrackToSkeletonMapTable());

    //for (int32 i = 0; i < MeshData->Bones.Num(); i++)
    //{
    //    const AssimpBone& Bone = MeshData->Bones[i];
    //    if (Bone.AnimTracks.Num())
    //    {
    //        const AssimpAnimationTrack& SrcTrack = Bone.AnimTracks[0];
    //        FRawAnimSequenceTrack NewTrack;
    //        NewTrack.PosKeys = SrcTrack.PosFrames;
    //        NewTrack.RotKeys = SrcTrack.RotFrames;
    //        NewTrack.ScaleKeys = SrcTrack.ScaleFrames;

    //        Tracks.Add(NewTrack);
    //        TrackToSkeletonMap.Add(FTrackToSkeletonMap(i));

    //        FrameCount = FMath::Max(FrameCount, SrcTrack.PosFrames.Num());
    //    }
    //}
    //Animation->SetRawNumberOfFrame(FrameCount);
    //
    //UE_LOG(LogTemp, Warning, TEXT("final track count %i"), Animation->GetRawAnimationData().Num());
    //
    //MeshComponent->PlayAnimation(Animation, true)
    return SkeletalMesh;
}

void ASkeletalRuntimeActorAsset::MaterialInitCallback(int32 index)
{
    MeshComponent->SetMaterial(index, MaterialInstances[index]);
    //SkeletalMesh->Materials[index] = MaterialInstances[index];
    //SkeletalMesh->Materials[index].UVChannelData.bInitialized = true;
}


#if USE_EDITOR_RENDER
void ASkeletalRuntimeActorAsset::GenerateSkeletalRenderMesh(TArray<FSoftSkinVertex>& Vertices, TArray<uint32>& Elements, FSkeletalMeshLODModel* LOD, FSkeletalMeshLODInfo& LODInfo, const RailwaysNode& Node, const int BoneCount)
{
#else
void ASkeletalRuntimeActorAsset::GenerateSkeletalRenderMesh(TArray<FSoftSkinVertex>& Vertices, TArray<uint32>& Elements, FSkeletalMeshLODRenderData* LOD, FSkeletalMeshLODInfo& LODInfo, const RailwaysNode& Node, const int BoneCount)
{
#endif
    const int32 NumMeshes = Node.Meshes.Num();
    //static int MeshIndex = 0;
    for (const RailwaysMesh& Mesh : Node.Meshes)
    {
#if USE_EDITOR_RENDER
        FSkelMeshSection& Section = *new(LOD->Sections) FSkelMeshSection();
#else
        FSkelMeshRenderSection& Section = *new(LOD->RenderSections) FSkelMeshRenderSection();
#endif
        
        //Section.MaterialIndex = MeshIndex;
        Section.NumVertices = Mesh.Vertices.Num();
        Section.NumTriangles = Mesh.Elements.Num() / 3;
        Section.MaxBoneInfluences = 4;

        TMap<int32, TArray<int32>> OverlappingVertices;

        const uint32 BaseVertIndex = Vertices.Num();
#if USE_EDITOR_RENDER
        Section.BaseVertexIndex = 0; //use the end of the buffer
        //Section.BaseIndex = 0;
#else
        Section.BaseVertexIndex = BaseVertIndex; //use the end of the buffer
        //Section.BaseIndex = Elements.Num();
#endif

        for (int t = 0; t < BoneCount; t++)
            Section.BoneMap.Add(t);

        Section.BaseIndex = Elements.Num();

        for (int32 VertIndex = 0; VertIndex < Mesh.Vertices.Num(); VertIndex++)
        {
            const auto& Vert = Mesh.Vertices[VertIndex];

            //skinned
            FSoftSkinVertex SkinnedVertex;
            SkinnedVertex.Position = Vert.Location;
            //SkinnedVertex.Position = FVector(0.0f);
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
            Section.BoneMap.AddUnique(0);

            TArray<int32> Overlap;
            for (int32 i = 0; i < FMath::Min(Vert.InfluenceBones.Num(), Section.MaxBoneInfluences); i++) //MAX_TOTAL_INFLUENCES
            {
                //UE_LOG(LogTemp, Warning, TEXT("bone %i weight %i"), Vert.InfluenceBones[i], Vert.InfluenceWeights[i]);
                SkinnedVertex.InfluenceBones[i] = Vert.InfluenceBones[i];
                SkinnedVertex.InfluenceWeights[i] = Vert.InfluenceWeights[i];

                Overlap.Add(Vert.InfluenceBones[i]);

                //Add to bone map
                //Section.BoneMap.AddUnique(SkinnedVertex.InfluenceBones[i]);
            }

#if USE_EDITOR_RENDER
            Section.SoftVertices.Add(SkinnedVertex);    
#endif
            //For LOD model vert count
            Vertices.Add(SkinnedVertex);

            OverlappingVertices.Add(VertIndex, Overlap); //Section.BaseIndex + 
        }
        
        //Section.OverlappingVertices
#if USE_EDITOR_RENDER
        Section.OverlappingVertices = OverlappingVertices;
#else
        Section.DuplicatedVerticesBuffer.Init(Section.NumVertices, OverlappingVertices);
#endif

        for (const uint32& element : Mesh.Elements)
            Elements.Add(BaseVertIndex + element); //BaseVertIndex + 

        Section.MaterialIndex = Mesh.MaterialIndex;
        LODInfo.LODMaterialMap.Add(Section.MaterialIndex);
        //SkeletalMesh->Materials.Add(UMaterial::GetDefaultMaterial(MD_Surface));
        //SkeletalMesh->Materials.Last().UVChannelData.bInitialized = true;

        


        UE_LOG(LogTemp, Warning, TEXT("added mesh %i"), Section.MaterialIndex);
        //MeshIndex++;
    }

    for (auto& Child : Node.Children)
        GenerateSkeletalRenderMesh(Vertices, Elements, LOD, LODInfo, Child, BoneCount);
}

