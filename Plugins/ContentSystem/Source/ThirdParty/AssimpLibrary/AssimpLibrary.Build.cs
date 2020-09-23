// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class AssimpLibrary : ModuleRules
{
	public AssimpLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "build", "include"));

			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "build", "code", "Release", "assimp-vc142-mt.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("assimp-vc142-mt.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/Assimp/assimp-vc142-mt.dll");
        }
        //else if (Target.Platform == UnrealTargetPlatform.Mac)
        //{
        //    PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
        //    RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/ContentSystemLibrary/Mac/Release/libExampleLibrary.dylib");
        //}
	}
}
