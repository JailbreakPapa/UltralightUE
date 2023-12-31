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

#pragma once

#include "Modules/ModuleManager.h"

class FPakFile;

class FUltralightUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/// @brief Loads Ultralight Resources Needed for Runtime. ( through package files)
	/// This will require the resource package containing the .dat, and the certification file that was included with the plugin.
	/// NOTE: Developers who choose to package ultralight assets with pak(s), will have to handle packaging those assets.
	/// It is recommended to use a native file interface for packaging resource files.
	bool LoadUltralightResources(FPakFile& p_resourcepak);
	/// @brief Loads Ultralight Resources Needed for Runtime. (through native filesystem)
	/// This will require the resource package containing the .dat, and the certification file that was included with the plugin.
	/// @param Path The path that the plugin will look for resource files. this CAN be empty, the plugin will assume the resources will be in: ({GAMEPATH}\\resources).
	bool LoadUltralightResources(FString& path);

	/// @brief App Core Library Path.
	FString AppCoreLibraryPath;
	/// @brief Web Core Library Path.
	FString WebCoreLibraryPath;
	/// @brief Ultralight Core Library Path.
	FString UltralightCoreLibraryPath;
	/// @brief Ultralight Library Path.
	FString UltralightLibraryPath;

private:
	/// @brief Destroys the Ultralight DLL handles. This should ONLY be called at shutdown.
	void DestroyUltralightHandles();

	/// Handles to Ultralight DLL(s).

	/// @brief Ultralight Core DLL Handle.
	void* UltralightCoreHandle;

	/// @brief Ultralight DLL Handle.
	void* UltralightHandle;

	/// @brief WebCore DLL Handle.
	void* WebCoreHandle;

	/// @brief AppCore DLL Handle.
	void* AppCoreHandle;
};
