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

#include "CoreMinimal.h"
#include "Ultralight/platform/FileSystem.h"


enum class ULTRALIGHTUE_API FSAccess : uint8_t
{
	FSA_Native = 0x0001, /// Native UE FileSystem, should be used with editor modules.
	FSA_Package = 0x0002, /// Package FileSystem, should be used at runtime.
};

/// @brief Unreal Engine implementation of Ultralight's FileSystem.
/*
 * NOTE: This is a internal class. you should not really be using this, unless you have a special filesystem to set.
 * HOW TO USE: SetFSAccess if you are using the native file system, or reading through package files, then provide to ULUEHandler.
 */
class ULTRALIGHTUE_API UEFileSystem : public ultralight::FileSystem
{
public:
	UEFileSystem(const FString& baseDir);
	virtual ~UEFileSystem() override;
	void RegisterVirtualFile(const FString& virtualPath, const TArray<uint8>& data);
	static void RegisterFile(const FString& VirtualPath, const TArray<uint8>& Data);
	virtual bool FileExists(const ultralight::String& file_path) override;
	virtual ultralight::String GetFileMimeType(const ultralight::String& file_path) override;
	virtual ultralight::String GetFileCharset(const ultralight::String& file_path) override;
	virtual ultralight::RefPtr<ultralight::Buffer> OpenFile(const ultralight::String& file_path) override;

private:
	FString BaseDir;

	// Helper to convert Ultralight String to Unreal FString
	FString ToFString(const ultralight::String& ulString) const;

	// Helper to get full file path
	FString GetFullPath(const ultralight::String& file_path) const;

	static TMap<FString, TArray<uint8>> VirtualFiles;
};
