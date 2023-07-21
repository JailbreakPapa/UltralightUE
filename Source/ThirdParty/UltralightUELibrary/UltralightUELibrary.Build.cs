/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class UltralightUELibrary : ModuleRules
{
    /// NOTE: If you do have access, feel free to add console/NDA based platforms here as well.
    // Credit for these two functions, that were added by a very nice UEForms user: https://forums.unrealengine.com/t/how-to-modify-build-file-to-copy-dlls-to-binaries/353587

    public string GetUProjectPath()
    {
        return Directory.GetParent(ModuleDirectory).Parent.Parent.ToString();
    }

    private void CopyToBinaries(string Filepath, ReadOnlyTargetRules Target)
    {
        string binariesDir = "";
        string filename = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Binaries", "Win64");
            filename = Path.GetFileName(Filepath);
        }

        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Mac", "Release");
            filename = Path.GetFileName(Filepath);
        }

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            binariesDir = Path.Combine(GetUProjectPath(), "Linux", "x86_64-unknown-linux-gnu");
            filename = Path.GetFileName(Filepath);
        }

        if (!Directory.Exists(binariesDir))
            Directory.CreateDirectory(binariesDir);

        if (!File.Exists(Path.Combine(binariesDir, filename)))
            File.Copy(Filepath, Path.Combine(binariesDir, filename), true);
    }
    public UltralightUELibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/include");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "AppCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "WebCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "UltralightCore.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "Ultralight.lib"));
            // Delay-load the DLL(s), so we can load it from the right place first
            PublicDelayLoadDLLs.Add("$(PluginDir)/Binaries/Win64/AppCore.dll");
            PublicDelayLoadDLLs.Add("$(PluginDir)/Binaries/Win64/WebCore.dll");
            PublicDelayLoadDLLs.Add("$(PluginDir)/Binaries/Win64/Ultralight.dll");
            PublicDelayLoadDLLs.Add("$(PluginDir)/Binaries/Win64/UltralightCore.dll");
            // Ensure that the DLL(s) is staged along with the executable
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUELibrary/Win64/AppCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUELibrary/Win64/WebCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUELibrary/Win64/UltralightCore.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UltralightUELibrary/Win64/Ultralight.dll");

            /// Copy Dll(s) over to binary directory, so the engine can actually load them.
            CopyToBinaries(ModuleDirectory + "/Win64/AppCore.dll", Target);
            CopyToBinaries(ModuleDirectory + "/Win64/WebCore.dll", Target);
            CopyToBinaries(ModuleDirectory + "/Win64/UltralightCore.dll", Target);
            CopyToBinaries(ModuleDirectory + "/Win64/Ultralight.dll", Target);
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libAppCore.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libUltralight.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libUltralightCore.dylib"));
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libWebCore.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/Mac/Release/libAppCore.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralight.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralightCore.dylib");
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UltralightUELibrary/Mac/Release/libWebCore.dylib");

            /// Copy Dll(s) over to binary directory, so the engine can actually load them.
            CopyToBinaries(ModuleDirectory + "/Mac/Release/AppCore.dylib", Target);
            CopyToBinaries(ModuleDirectory + "/Mac/Release/WebCore.dylib", Target);
            CopyToBinaries(ModuleDirectory + "/Mac/Release/UltralightCore.dylib", Target);
            CopyToBinaries(ModuleDirectory + "/Mac/Release/Ultralight.dylib", Target);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string SoWebCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUELibrary", "Linux", "x86_64-unknown-linux-gnu", "libWebCore.so");
            string SoULCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUELibrary", "Linux", "x86_64-unknown-linux-gnu", "libUltralightCore.so");
            string SoULPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUELibrary", "Linux", "x86_64-unknown-linux-gnu", "libUltralight.so");
            string SoAppCorePath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "UltralightUELibrary", "Linux", "x86_64-unknown-linux-gnu", "libAppCore.so");
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

            /// Copy Dll(s) over to binary directory, so the engine can actually load them.
            /// NOTE: Directory is assumed here. will have to test more on linux.
            CopyToBinaries(ModuleDirectory + "/Linux/x86_64-unknown-linux-gnu/libWebCore.so", Target);
            CopyToBinaries(ModuleDirectory + "/Linux/x86_64-unknown-linux-gnu/libAppCore.so", Target);
            CopyToBinaries(ModuleDirectory + "/Linux/x86_64-unknown-linux-gnu/libUltralightCore.so", Target);
            CopyToBinaries(ModuleDirectory + "/Linux/x86_64-unknown-linux-gnu/Ultralight.so", Target);
        }
    }
}