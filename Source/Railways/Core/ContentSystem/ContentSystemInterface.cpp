// Fill out your copyright notice in the Description page of Project Settings.


#include "ContentSystemInterface.h"

static UContentSystemInterface* instance;
static FCriticalSection mutex;

UContentSystemInterface* UContentSystemInterface::GetInterface()
{
	FScopeLock ScopeLock(&mutex);
	if (!instance)
		instance = NewObject<UContentSystemInterface>();

	return instance;
}

void UContentSystemInterface::ShutdownInterface()
{
	FScopeLock ScopeLock(&mutex);

	UE_LOG(LogTemp, Log, TEXT("Content system shutdown requested?"));
	//should be automatically cleaned up when the reference is gone
	instance = nullptr;
}

FContentInfo UContentSystemInterface::RetrieveContentData(FString ContentID)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FContentInfo Data;
	FString SourceDir = FPaths::Combine(ActiveDir, ContentID);
	//download content if not locally installed

	//get json metadata

	if (PlatformFile.DirectoryExists(*SourceDir))
	{
		Data.ContentDir = SourceDir;


		UE_LOG(LogTemp, Display, TEXT("Successfully loaded content %s"), *ContentID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find content directory for asset %s"), *ContentID);
	}
	return Data;
}

UContentSystemInterface::UContentSystemInterface()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing Railways content system."));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	ContentDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RailwaysContent"));
	if (!PlatformFile.DirectoryExists(*ContentDir)) PlatformFile.CreateDirectory(*ContentDir);

	EditDir = FPaths::Combine(ContentDir, TEXT("Editing"));
	ActiveDir = FPaths::Combine(ContentDir, TEXT("Active"));
	if (!PlatformFile.DirectoryExists(*EditDir)) PlatformFile.CreateDirectory(*EditDir);
	if (!PlatformFile.DirectoryExists(*ActiveDir)) PlatformFile.CreateDirectory(*ActiveDir);


	UE_LOG(LogTemp, Warning, TEXT("CONTENT DIR: %s"), *EditDir);
}

UContentSystemInterface::~UContentSystemInterface()
{
	UE_LOG(LogTemp, Log, TEXT("Shutting down Railways content system."));
}
