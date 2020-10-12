// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContentSystem.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
//#include "ContentSystemLibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FContentSystemModule"

void FContentSystemModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("ContentSystem")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/ThirdParty/AssimpLibrary/assimp-vc142-mt.dll"));
#elif PLATFORM_MAC
    //LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/ContentSystemLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS

	//LibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	//if (LibraryHandle)
	//{
	//	// Call the test function in the third party library that opens a message box


	//}
	//else
	//{
	//	if (LibraryPath.IsEmpty())
	//	{
	//		FText ErrorMsg = FText::Format(LOCTEXT("ThirdPartyLibraryError", "Could not find Assimp DLL {0}!"), FText::FromString(LibraryPath));
	//		FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
	//	}
	//	else
	//	{
	//		FText ErrorMsg = FText::Format(LOCTEXT("ThirdPartyLibraryError", "Failed to load Assimp from directory {0}!"), FText::FromString(LibraryPath));
	//		FMessageDialog::Open(EAppMsgType::Ok, ErrorMsg);
	//	}
	//}
}

void FContentSystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(LibraryHandle);
	LibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FContentSystemModule, ContentSystem)
