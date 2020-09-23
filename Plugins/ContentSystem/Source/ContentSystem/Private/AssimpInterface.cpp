// Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpInterface.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


AssimpImportData* UAssimpInterface::ImportFBX()
{
	auto FileDir = FPaths::ProjectPluginsDir() + "Content/Samples/icosphere.fbx";
	UE_LOG(LogTemp, Warning, TEXT("Importing FBX file %s"), *FileDir);

	return nullptr;
}