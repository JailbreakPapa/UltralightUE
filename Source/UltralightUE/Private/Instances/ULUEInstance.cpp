#include "Instances/ULUEInstance.h"
#include "UltralightUE.h"
#include "Rendering/ULUERenderManagerCPU.h"
#include "Engine/GameInstance.h"
#include "Misc/Paths.h"
#include "Tickable.h"
#include "UltralightUESettings.h"
#include "FileSystem/ULUEFileSystem.h"
#include "FontSystem/ULUEFontSystem.h"


ULUEInstance::ULUEInstance()
{
	RenderManager = nullptr;
	FileSystem = nullptr;
	FontLoader = nullptr;
	
}

void ULUEInstance::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Load settings from UltralightUE.ini
	const UUltralightUESettings* Settings = GetDefault<UUltralightUESettings>();
	FString BaseDir = FPaths::Combine(FPaths::ProjectContentDir(), Settings->ResourcesPath);
	if (!BaseDir.EndsWith(TEXT("/")))
	{
		BaseDir += TEXT("/");
	}
	// Setup Platform Instance
	PlatformManager = MakeShared<ULUEPlatformManager>(Fonts);
	PlatformManager->Initialize();
	RenderManager = NewObject<ULUERenderManagerCPU>(this);
	if (!RenderManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ULUEInstance: Failed to create ULUERenderManagerCPU!"));
		return;
	}

	// Subscribe to world initialization to handle transitions
	WorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
		this, &ULUEInstance::OnPostWorldInitialization);

	// Subscribe to tick for the current world
	if (UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull))
	{
		WorldTickHandle = FWorldDelegates::OnWorldTickStart.AddUObject(this, &ULUEInstance::OnWorldTick);
	}
}

void ULUEInstance::Deinitialize()
{
	// Unsubscribe from delegates
	if (WorldTickHandle.IsValid())
	{
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
		WorldTickHandle.Reset();
	}

	if (WorldInitHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(WorldInitHandle);
		WorldInitHandle.Reset();
	}

	if (RenderManager)
	{
		RenderManager->ConditionalBeginDestroy();
		RenderManager = nullptr;
	}

	if (FileSystem)
	{
		delete FileSystem;
		FileSystem = nullptr;
	}

	if (FontLoader)
	{
		delete FontLoader;
		FontLoader = nullptr;
	}

	Super::Deinitialize();
}

ULUERenderManagerCPU* ULUEInstance::GetRenderManager() const
{
	return RenderManager;
}

void ULUEInstance::OnWorldTick(UWorld* World, ELevelTick TickType, float DeltaTime)
{
	if (RenderManager && TickType
		==
		LEVELTICK_All
	)
	{
		RenderManager->Tick(DeltaTime);
		UE_LOG(LogTemp, Verbose, TEXT("ULUEInstance: Ticking RenderManager"));
	}
}

void ULUEInstance::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	// Re-subscribe to tick for the new world
	if (WorldTickHandle.IsValid())
	{
		FWorldDelegates::OnWorldTickStart.Remove(WorldTickHandle);
		WorldTickHandle.Reset();
	}

	WorldTickHandle = FWorldDelegates::OnWorldTickStart.AddUObject(this, &ULUEInstance::OnWorldTick);
	UE_LOG(LogTemp, Log, TEXT("ULUEInstance: Subscribed to tick for new world"));
}
