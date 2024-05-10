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

#include "../../PakFile/Public/IPlatformFilePak.h"
#include "Modules/ModuleManager.h"

namespace ultralightue
{
	class ULUELogInterface;
}

class FUltralightUEModule final : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FUltralightUEModule &Get()
	{
		return FModuleManager::LoadModuleChecked<FUltralightUEModule>("UltralightUE");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable();

	/// @brief Loads Ultralight Resources Needed for Runtime. (through package files)
	/// This will require the resource package containing the .dat, and the certification file that was included with the plugin.
	/// NOTE: Developers who choose to package ultralight assets with pak(s), will have to handle packaging those assets.
	/// It is recommended to use a native file interface for packaging resource files.
	static bool CheckUltralightResources(FPakFile &p_resourcepak);

	/// @brief Loads Ultralight Resources Needed for Runtime. (through native filesystem)
	/// This will require the resource package containing the .dat, and the certification file that was included with the plugin.
	/// @param Path The path that the plugin will look for resource files. this CAN be empty, the plugin will assume the resources will be in: ({GAMECONTENTPATH}\\uicontent).
	bool CheckUltralightResources(FString &path) const;

	/// @brief Sets & Uses the Logging interface to integrate with Ultralight.
	/// @param in_logginginterface Interface to be used.
	void SetLoggingInterface(ultralightue::ULUELogInterface &in_logginginterface);

	/// @brief Get the log interface.
	/// @return
	ultralightue::ULUELogInterface* GetLogInterface() const;
	/// @brief Web Core Library Path.
	FString WebCoreLibraryPath;
	/// @brief Ultralight Core Library Path.
	FString UltralightCoreLibraryPath;
	/// @brief Ultralight Library Path.
	FString UltralightLibraryPath;

private:
	/// @brief Destroys the Ultralight DLL handles. This should ONLY be called at shutdown.
	void DestroyUltralightHandles();

	ultralightue::ULUELogInterface* m_loginterface = nullptr;
	/// Handles to Ultralight DLL(s).

	/// @brief Ultralight Core DLL Handle.
	void *UltralightCoreHandle;

	/// @brief Ultralight DLL Handle.
	void *UltralightHandle;

	/// @brief WebCore DLL Handle.
	void *WebCoreHandle;
};
