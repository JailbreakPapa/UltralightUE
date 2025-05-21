#include "Instances/ULUEFontSubsystem.h"
#include "Assets/ULUEFontAsset.h"
#include "FileSystem/ULUEFileSystem.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"

void ULUEFontSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RegisterFontsWithFileSystem();
}

void ULUEFontSubsystem::Deinitialize()
{
	Super::Deinitialize();
	// Cleanup if needed
}

void ULUEFontSubsystem::RegisterFontsWithFileSystem()
{
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UULUEFontAsset::StaticClass(), false, true);
	// TODO(Mikael A.): Make this configurable...
	ObjectLibrary->LoadAssetDataFromPath(TEXT("/uiresources/fonts")); // Adjust the path as needed

	TArray<FAssetData> AssetData;
	ObjectLibrary->GetAssetDataList(AssetData);

	for (const FAssetData& Asset : AssetData)
	{
		UULUEFontAsset* FontAsset = Cast<UULUEFontAsset>(Asset.GetAsset());
		if (FontAsset && FontAsset->FontData.Num() > 0)
		{
			FString VirtualPath = FontAsset->GetVirtualFilePath();
			// Register font data with UEFileSystem (implementation depends on your UEFileSystem, if its custom.)
			UEFileSystem::RegisterFile(VirtualPath, FontAsset->FontData);
			UE_LOG(LogTemp, Log, TEXT("UltralightUE: Registered font: %s"), *VirtualPath);
		}
	}
}
