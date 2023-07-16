// Fill out your copyright notice in the Description page of Project Settings.

using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class UltralightUELibrary : ModuleRules
{
    public UltralightUELibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/include");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "AppCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "WebCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "UltralightCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "Ultralight.lib"));
            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("AppCore.dll");
            PublicDelayLoadDLLs.Add("WebCore.dll");
            PublicDelayLoadDLLs.Add("Ultralight.dll");
            PublicDelayLoadDLLs.Add("UltralightCore.dll");
            // Ensure that the DLL(s) is staged along with the executable
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUE/Win64/AppCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUE/Win64/WebCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUE/Win64/UltralightCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUE/Win64/Ultralight.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libAppCore.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libUltralight.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libUltralightCore.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libWebCore.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUE/Mac/Release/libAppCore.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUE/Mac/Release/libUltralight.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUE/Mac/Release/libUltralightCore.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUE/Mac/Release/libWebCore.dylib");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string SoWebCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUE", "Linux", "x86_64-unknown-linux-gnu", "libWebCore.so");
            string SoULCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUE", "Linux", "x86_64-unknown-linux-gnu", "libUltralightCore.so");
            string SoULPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUE", "Linux", "x86_64-unknown-linux-gnu", "libUltralight.so");
            string SoAppCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUE", "Linux", "x86_64-unknown-linux-gnu", "libAppCore.so");
            PublicAdditionalLibraries.Add(SoWebCorePath);
            PublicAdditionalLibraries.Add(SoAppCorePath);
            PublicAdditionalLibraries.Add(SoULCorePath);
            PublicAdditionalLibraries.Add(SoULPath);
            PublicDelayLoadDLLs.Add(SoWebCorePath);
            PublicDelayLoadDLLs.Add(SoAppCorePath);
            PublicDelayLoadDLLs.Add(SoULCorePath);
            PublicDelayLoadDLLs.Add(SoULPath);
            RuntimeDependencies.Add(SoWebCorePath);
            RuntimeDependencies.Add(SoAppCorePath);
            RuntimeDependencies.Add(SoULCorePath);
            RuntimeDependencies.Add(SoULPath);
        }
    }
}