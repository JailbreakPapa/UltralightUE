// Copyright Epic Games, Inc. All Rights Reserved.

#include "UltralightUE.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "UltralightUELibrary/include/Ultralight/Renderer.h"
#include "UltralightUELibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FUltralightUEModule"

void FUltralightUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("UltralightUE")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUE/Win64/AppCore.dll"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUE/Win64/WebCore.dll"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUE/Win64/UltralightCore.dll"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUE/Win64/Ultralight.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libAppCore.dylib"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralight.dylib"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libUltralightCore.dylib"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/UltralightUELibrary/Mac/Release/libWebCore.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/UltralightUELibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box, notifying the user that the library has opened.
		UltralightUEFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UltralightUE: Error", "Failed to load UltralightUE! Please check the log for any messages. if you cant fix the issue, create a issue on github!"));
	}
}

void FUltralightUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltralightUEModule, UltralightUE)