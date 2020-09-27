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

	//should be automatically cleaned up when the reference is gone
	instance = nullptr;
}

UContentSystemInterface::UContentSystemInterface()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing Railways content system."));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	ContentDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RailwaysContent"));
	if (!PlatformFile.DirectoryExists(*ContentDir))
		PlatformFile.CreateDirectory(*ContentDir);

	UE_LOG(LogTemp, Warning, TEXT("CONTENT DIR: %s"), *ContentDir);
}

UContentSystemInterface::~UContentSystemInterface()
{
	UE_LOG(LogTemp, Log, TEXT("Shutting down Railways content system."));
}
