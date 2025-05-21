/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

using System.IO;
using UnrealBuildTool;

public class UltralightUE : ModuleRules
{
	public UltralightUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// Add include paths for UltralightUELibrary and Ultralight SDK
		PublicIncludePaths.AddRange(
			new[]
			{
				// UltralightUELibrary module include path
				Path.Combine(ModuleDirectory, "../../Source/ThirdParty/UltralightUELibrary"),
				// Ultralight SDK include path
				Path.Combine(ModuleDirectory, "../../Source/ThirdParty/UltralightUELibrary/Include")
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
				// Add private include paths if needed
			}
		);

		// Consolidated dependencies
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Renderer",
				"RenderCore",
				"Projects",
				"PakFile",
				"RSA",
				"UltralightUELibrary" // Dependency on UltralightUELibrary module
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Add private dependencies if needed
			}
		);
	}
}