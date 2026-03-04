#include "FileSystem/ULUEFileSystem.h" // Corrected relative path

#include "Ultralight/Ultralight.h" // This comes from the ThirdParty library
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
// Removed duplicate include of "ULUEFileSystem.h"

namespace ultralightue
{
    void ULUEFileSystem::SetFSAccess(FSAccess InAccessPattern)
    {
        m_access = InAccessPattern;
    }

    ULUEFileSystem::ULUEFileSystem()
    {
        // TODO: Load the base directory from the config file set by the user.
    }

    bool ULUEFileSystem::FileExists(const ultralight::String &path)
    {
        FString uePath = MapPath(path);
        IPlatformFile &platformFile = FPlatformFileManager::Get().GetPlatformFile();
        return platformFile.FileExists(*uePath);
    }
    bool ULUEFileSystem::GetFileSize(ultralight::FileHandle handle, int64_t &result)
    {
        IFileHandle *fileHandle = static_cast<IFileHandle *>(handle);
        if (fileHandle)
        {
            result = fileHandle->Size();
            return true;
        }
        return false;
    }
    ultralight::FileHandle ULUEFileSystem::OpenFile(const ultralight::String &path, bool open_for_writing)
    {
        FString uePath = MapPath(path);
        IPlatformFile &platformFile = FPlatformFileManager::Get().GetPlatformFile();
        IFileHandle *handle = nullptr;
        if (open_for_writing)
        {
            handle = platformFile.OpenWrite(*uePath);
        }
        else
        {
            handle = platformFile.OpenRead(*uePath);
        }
        return handle ? static_cast<ultralight::FileHandle>(handle) : nullptr;
    }
    void ULUEFileSystem::CloseFile(ultralight::FileHandle &handle)
    {
        IFileHandle *fileHandle = static_cast<IFileHandle *>(handle);
        if (fileHandle)
        {
            delete fileHandle; // Unreal Engine requires manual deletion of IFileHandle
            handle = nullptr;
        }
    }
    int64_t ULUEFileSystem::ReadFromFile(ultralight::FileHandle handle, char* data, int64_t length)
    {
        IFileHandle* fileHandle = static_cast<IFileHandle*>(handle);
        if (fileHandle)
        {
            // UE's IFileHandle::Read returns bool, but Ultralight's ReadFromFile (version 1.3+) expects int64_t (bytes read).
            // We need to simulate bytes read. If Read is successful, assume all 'length' bytes were read.
            // If underlying Read can return actual bytes read, that would be more accurate.
            if (fileHandle->Read(reinterpret_cast<uint8_t*>(data), length)) {
                return length; // Assuming full read on success
            } else {
                return -1; // Indicate error as per Ultralight docs (negative for error)
            }
        }
        return -1; // Indicate error
    }

    // Destructor implementation (if not already present or defaulted in header)
    ULUEFileSystem::~ULUEFileSystem()
    {
        // Cleanup if necessary, though typically not needed for FileSystem interfaces
        // unless it owns resources that need explicit release not handled by handles.
    }

    bool ULUEFileSystem::GetFileMimeType(const ultralight::String &path, ultralight::String &result)
    {
        FString uePath = FString(path.utf8().data());
        if (uePath.EndsWith(TEXT(".html")))
        {
            result = ultralight::String("text/html");
            return true;
        }
        else if (uePath.EndsWith(TEXT(".css")))
        {
            result = ultralight::String("text/css");
            return true;
        }
        else if (uePath.EndsWith(TEXT(".js")))
        {
            result = ultralight::String("application/javascript");
            return true;
        }
        // NOTE/TODO (Mikael A.): We could add more but, these are the most common ones.
        result = ultralight::String("application/octet-stream");
        return true;
    }
    bool ULUEFileSystem::GetFileCharset(const ultralight::String &path, ultralight::String &result)
    {
        result = ultralight::String("utf-8");
        return true;
    }
    // (Mikael A.): I mean, i dont think UL will EVER use this, but we can add it for completeness.
    bool ULUEFileSystem::GetFileCreationTime(const ultralight::String &path, int64_t &result)
    {
        FString uePath = MapPath(path);
        IPlatformFile &platformFile = FPlatformFileManager::Get().GetPlatformFile();
        FDateTime creationTime;
        if (platformFile.GetTimeStamp(*uePath, creationTime))
        {
            result = creationTime.ToUnixTimestamp();
            return true;
        }
        return false;
    }
    // (Mikael A.): I mean, i dont think UL will EVER use this, but we can add it for completeness.
    bool ULUEFileSystem::GetFileLastModificationTime(const ultralight::String &path, int64_t &result)
    {
        FString uePath = MapPath(path);
        IPlatformFile &platformFile = FPlatformFileManager::Get().GetPlatformFile();
        FDateTime modificationTime;
        if (platformFile.GetTimeStamp(*uePath, modificationTime))
        {
            result = modificationTime.ToUnixTimestamp();
            return true;
        }
        return false;
    }
    // Map Ultralight path to Unreal Engine virtual path
    FString ULUEFileSystem::MapPath(const ultralight::String& path)
    {
        FString ultralightPath = FString(path.utf8().data());
        // Remove any leading slashes to avoid double slashes
        if (ultralightPath.StartsWith(TEXT("/")))
        {
            ultralightPath = ultralightPath.Mid(1);
        }
        return BaseDirectory / ultralightPath;
    }
}