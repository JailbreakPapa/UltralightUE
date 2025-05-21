#include "FileSystem/ULUEFileSystem.h"
#include <Ultralight/Ultralight.h>
#include "InterchangeHelper.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformMisc.h"

TMap<FString, TArray<uint8>> UEFileSystem::VirtualFiles;

UEFileSystem::UEFileSystem(const FString& baseDir)
	: BaseDir(baseDir)
{
	// Ensure BaseDir ends with a separator
	if (!BaseDir.EndsWith(TEXT("/")) && !BaseDir.EndsWith(TEXT("\\")))
	{
		BaseDir += TEXT("/");
	}
}

UEFileSystem::~UEFileSystem()
{
}

void UEFileSystem::RegisterVirtualFile(const FString& virtualPath, const TArray<uint8>& data)
{
	VirtualFiles.Add(virtualPath, data);
}

bool UEFileSystem::FileExists(const ultralight::String& file_path)
{
	FString fullPath = GetFullPath(file_path);
	return FPaths::FileExists(fullPath);
}

ultralight::String UEFileSystem::GetFileMimeType(const ultralight::String& file_path)
{
	FString path = ToFString(file_path).ToLower();
	FString extension = FPaths::GetExtension(path, false);

	if (extension == TEXT("html") || extension == TEXT("htm"))
	{
		return ultralight::String("text/html");
	}
	if (extension == TEXT("css"))
	{
		return ultralight::String("text/css");
	}
	if (extension == TEXT("js"))
	{
		return ultralight::String("application/javascript");
	}
	if (extension == TEXT("png"))
	{
		return ultralight::String("image/png");
	}
	if (extension == TEXT("jpg") || extension == TEXT("jpeg"))
	{
		return ultralight::String("image/jpeg");
	}
	if (extension == TEXT("gif"))
	{
		return ultralight::String("image/gif");
	}
	if (extension == TEXT("svg"))
	{
		return ultralight::String("image/svg+xml");
	}
	if (extension == TEXT("txt"))
	{
		return ultralight::String("text/plain");
	}

	return ultralight::String("application/unknown");
}

ultralight::String UEFileSystem::GetFileCharset(const ultralight::String& file_path)
{
	// For simplicity, return "utf-8" for text-based files
	FString path = ToFString(file_path).ToLower();
	FString extension = FPaths::GetExtension(path, false);

	if (extension == TEXT("html") || extension == TEXT("htm") ||
		extension == TEXT("css") || extension == TEXT("js") ||
		extension == TEXT("txt"))
	{
		return ultralight::String("utf-8");
	}

	return ultralight::String("utf-8"); // Default safe charset
}

ultralight::RefPtr<ultralight::Buffer> UEFileSystem::OpenFile(const ultralight::String& file_path)
{
	FString fullPath = GetFullPath(file_path);
	TArray<uint8> fileData;

	if (!FFileHelper::LoadFileToArray(fileData, *fullPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("UEFileSystem: Failed to load file %s"), *fullPath);
		return nullptr;
	}

	// Allocate aligned memory for ICU data file (16-byte alignment)
	bool isIcuFile = fullPath.EndsWith(TEXT("icudt67l.dat"), ESearchCase::IgnoreCase);
	size_t dataSize = fileData.Num();
	void* dataPtr = nullptr;

	if (isIcuFile)
	{
		// Use aligned allocation for ICU data
		dataPtr = FMemory::Malloc(dataSize, 16); // 16-byte alignment
		if (!dataPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("UEFileSystem: Failed to allocate aligned memory for %s"), *fullPath);
			return nullptr;
		}
		FMemory::Memcpy(dataPtr, fileData.GetData(), dataSize);
	}
	else
	{
		// For non-ICU files, use Buffer::CreateFromCopy to ensure alignment
		dataPtr = FMemory::Malloc(dataSize);
		if (!dataPtr)
		{
			UE_LOG(LogTemp, Error, TEXT("UEFileSystem: Failed to allocate memory for %s"), *fullPath);
			return nullptr;
		}
		FMemory::Memcpy(dataPtr, fileData.GetData(), dataSize);
	}

	// Create Buffer...
	return ultralight::Buffer::Create(dataPtr, dataSize, nullptr, nullptr);
}

FString UEFileSystem::ToFString(const ultralight::String& ulString) const
{
	// Convert Ultralight String (UTF-16) to FString
	return FString(UTF16_TO_TCHAR(ulString.utf16().data()));
}

FString UEFileSystem::GetFullPath(const ultralight::String& file_path) const
{
	FString relativePath = ToFString(file_path);
	// Replace forward slashes with platform-specific separator
	relativePath.ReplaceInline(TEXT("/"), FGenericPlatformMisc::GetDefaultPathSeparator());
	return FPaths::Combine(BaseDir, relativePath);
}


void UEFileSystem::RegisterFile(const FString& VirtualPath, const TArray<uint8>& Data)
{
	VirtualFiles.Add(VirtualPath, Data);
}
