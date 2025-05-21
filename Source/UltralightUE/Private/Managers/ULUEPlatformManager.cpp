#include "Managers/ULUEPlatformManager.h"
#include "UltralightUE.h"
#include "FileSystem/ULUEFileSystem.h"
#include <Ultralight/Ultralight.h>

void ULUEPlatformManager::Initialize()
{
	m_Config.resource_path_prefix = "uiresources/";
	ultralight::Platform::instance().set_config(m_Config);
	if (FUltralightUEModule::IsAvailable())
	{
		ultralight::Platform::instance().set_file_system(FUltralightUEModule::Get().GetFileSystem());
		
	}
}

void ULUEPlatformManager::Deinitialize()
{
}
