// Fill out your copyright notice in the Description page of Project Settings.


#include "ContentSystemInterface.h"

//ContentSystemInterface::ContentSystemInterface()
//{
//	UE_LOG(LogTemp, Log, TEXT("Initializing Railways content system."));
//	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
//	ContentDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RailwaysContent"));
//	if (!PlatformFile.DirectoryExists(*ContentDir))
//		PlatformFile.CreateDirectory(*ContentDir);
//
//	UE_LOG(LogTemp, Warning, TEXT("CONTENT DIR: %s"), *ContentDir);
//}
//
//ContentSystemInterface::~ContentSystemInterface()
//{
//	UE_LOG(LogTemp, Log, TEXT("Shutting down Railways content system."));
//}
//
//static FCriticalSection mutex;
//static ContentSystemInterface instance;
//
//CONTENTSYSTEM_API ContentSystemInterface* GetContentSystem()
//{
//	FScopeLock ScopeLock(&mutex);
//	return &instance;
//}
