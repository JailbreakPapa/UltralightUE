#include "Managers/ULUEPlatformManager.h"
#include "FileSystem/ULUEFileSystem.h"
#include "UltralightUE.h"
#include "ULUELogInterface.h"
#include "FontSystem/ULUEFontSystem.h"
#include "Internal/ULUEILoggerInterface.h"

#include <Ultralight/Ultralight.h>

#include "UltralightUESettings.h"
#include "Engine/ObjectLibrary.h"

ULUEPlatformManager::ULUEPlatformManager(TArray<UULUEFontAsset*>& in_Fonts)
{
	const UUltralightUESettings* Settings = GetDefault<UUltralightUESettings>();
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UULUEFontAsset::StaticClass(), false, true);
	ObjectLibrary->LoadAssetDataFromPath(FString::Printf(TEXT("/Game/%s/%s"), *Settings->ResourcesPath, *Settings->FontsPath));
	ObjectLibrary->GetObjects(in_Fonts);
	FUltralightUEModule::Get().m_fontsystem = MakeShared<ULUEFontSystem>(in_Fonts, Settings->FallbackFontFamily);
}

ULUEPlatformManager::~ULUEPlatformManager()
{
}

void ULUEPlatformManager::Initialize()
{
	m_Config = MakeUnique<ultralight::Config>().Get();
	m_Config->resource_path_prefix = "uiresources/";
	ultralight::Platform::instance().set_config(*m_Config.Get());
	if (FUltralightUEModule::IsAvailable())
	{
		ultralight::Platform::instance().set_file_system(FUltralightUEModule::Get().GetFileSystem());
		ultralight::Platform::instance().set_logger(FUltralightUEModule::Get().GetLogInterface()->GetInternalInterface());
		// Start Setting up the font loader. Theoretically, the setup work in the constructor should get this ready.
		ultralight::Platform::instance().set_font_loader(FUltralightUEModule::Get().GetFontSystem());
		FUltralightUEModule::Get().SetupRenderer();
	}
}

void ULUEPlatformManager::Deinitialize()
{
}

ultralight::Config* ULUEPlatformManager::GetConfig()
{
	return m_Config.Get();
}
