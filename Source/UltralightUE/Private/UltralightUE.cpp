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

#include "UltralightUE.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "UltralightUELibrary/ULUELibrary.h"

#define LOCTEXT_NAMESPACE "FUltralightUEModule"

void FUltralightUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("UltralightUE")->GetBaseDir();

	// Add on the relative location of the ultralight dll(s) and load them.
#if PLATFORM_WINDOWS
	AppCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/AppCore.dll"));
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/WebCore.dll"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/UltralightCore.dll"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/Ultralight.dll"));
#elif PLATFORM_MAC
	AppCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libAppCore.dylib"));
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralight.dylib"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralightCore.dylib"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libWebCore.dylib"));
#elif PLATFORM_LINUX
	AppCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libWebCore.so"));
	WebCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libUltralightCore.so"));
	UltralightCoreLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libUltralight.so"));
	UltralightLibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libAppCore.so"));
#endif // PLATFORM_WINDOWS
	/// Assign handles to Library(s) path(s).

	AppCoreHandle = !AppCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*AppCoreLibraryPath) : nullptr;
	WebCoreHandle = !WebCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*WebCoreLibraryPath) : nullptr;
	UltralightCoreHandle = !UltralightCoreLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*UltralightCoreLibraryPath) : nullptr;
	UltralightHandle = !UltralightLibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*UltralightLibraryPath) : nullptr;

	if (AppCoreHandle && WebCoreHandle && UltralightCoreHandle && UltralightHandle != nullptr)
	{
		// Call the function in the Ultralight library that opens a message box, notifying the user that the library has opened.
		ultralightue::UltralightUEStartup();
		// TODO: Startup Ultralight engine.
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UltralightUE: Error", "Failed to load UltralightUE! Please check the log for any messages. if you cant fix the issue, create a issue on github! (https://github.com/JailbreakPapa/UltralightUE)"));
	}
}

void FUltralightUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handles to ultralight dll(s).
	DestroyUltralightHandles();
}

bool FUltralightUEModule::LoadUltralightResources(FPakFile& p_resourcepak)
{
	/// Looks like we were able to get all needed resources.
	return true;
}

bool FUltralightUEModule::LoadUltralightResources(FString& path)
{
	/// Looks like we were able to get all needed resources.
	return true;
}

void FUltralightUEModule::DestroyUltralightHandles()
{
	FPlatformProcess::FreeDllHandle(UltralightHandle);
	FPlatformProcess::FreeDllHandle(UltralightCoreHandle);
	FPlatformProcess::FreeDllHandle(WebCoreHandle);
	FPlatformProcess::FreeDllHandle(AppCoreHandle);
	UltralightHandle = nullptr;
	UltralightCoreHandle = nullptr;
	WebCoreHandle = nullptr;
	AppCoreHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltralightUEModule, UltralightUE)