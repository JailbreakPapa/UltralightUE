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

#include "ULUESubsystem.h"
#include "ULUELogging.h"
#include "ULUEViewEntry.h"

// Ultralight SDK headers (only in this .cpp)
#include <Ultralight/Ultralight.h>
#include <Ultralight/Renderer.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

// Platform implementations
#include "Platform/ULUEFontLoader.h"
#include "Platform/ULUEClipboard.h"
#include "FileSystem/ULUEFileSystem.h"

// UE includes
#include "Engine/Texture2D.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "TextureResource.h"

// ---------------------------------------------------------------------------
// Helper: convert FString -> ultralight::String (UTF-8)
// ---------------------------------------------------------------------------
static ultralight::String FStringToULString(const FString& InString)
{
	FTCHARToUTF8 Converter(*InString);
	return ultralight::String(Converter.Get(), Converter.Length());
}

// ---------------------------------------------------------------------------
// Helper: convert ultralight::String -> FString
// ---------------------------------------------------------------------------
static FString ULStringToFString(const ultralight::String& InString)
{
	return FString(UTF8_TO_TCHAR(InString.utf8().data()));
}

// ===========================================================================
// Typed accessor helpers for the opaque pointers
// ===========================================================================

static ultralight::RefPtr<ultralight::Renderer>& GetRenderer(void*& Ptr)
{
	return *static_cast<ultralight::RefPtr<ultralight::Renderer>*>(Ptr);
}

// ===========================================================================
// USubsystem interface
// ===========================================================================

void UUltralightSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogUltralightUE, Log, TEXT("UUltralightSubsystem::Initialize - Setting up Ultralight platform."));

	SetupPlatform();

	bIsInitialized = true;

	UE_LOG(LogUltralightUE, Log, TEXT("UUltralightSubsystem::Initialize - Complete."));
}

void UUltralightSubsystem::Deinitialize()
{
	UE_LOG(LogUltralightUE, Log, TEXT("UUltralightSubsystem::Deinitialize - Tearing down."));

	// Destroy all views first
	TArray<FName> ViewNames;
	Views.GetKeys(ViewNames);
	for (const FName& Name : ViewNames)
	{
		DestroyView(Name);
	}

	TeardownPlatform();

	bIsInitialized = false;

	Super::Deinitialize();

	UE_LOG(LogUltralightUE, Log, TEXT("UUltralightSubsystem::Deinitialize - Complete."));
}

// ===========================================================================
// FTickableGameObject interface
// ===========================================================================

void UUltralightSubsystem::Tick(float DeltaTime)
{
	if (!RendererPtr)
	{
		return;
	}

	ultralight::RefPtr<ultralight::Renderer>& Renderer = GetRenderer(RendererPtr);

	// Drive the Ultralight update & render loop
	Renderer->Update();
	Renderer->Render();

	// Copy CPU-rendered surfaces into UTexture2D for each non-accelerated view
	for (auto& Pair : Views)
	{
		FULUEViewEntry& Entry = *Pair.Value;
		if (!Entry.bIsAccelerated && Entry.View.get() && Entry.UETexture)
		{
			UpdateCPUTexture(Entry);
		}
	}
}

TStatId UUltralightSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUltralightSubsystem, STATGROUP_Tickables);
}

// ===========================================================================
// View management
// ===========================================================================

FName UUltralightSubsystem::CreateView(FName ViewName, const FUltralightViewConfig& Config)
{
	if (!RendererPtr)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateView: Renderer not initialised."));
		return NAME_None;
	}

	if (Views.Contains(ViewName))
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("CreateView: View '%s' already exists."), *ViewName.ToString());
		return ViewName;
	}

	ultralight::RefPtr<ultralight::Renderer>& Renderer = GetRenderer(RendererPtr);

	// Build Ultralight ViewConfig
	ultralight::ViewConfig ViewCfg;
	ViewCfg.is_accelerated = (Config.RenderMode == EUltralightRenderMode::GPU);
	ViewCfg.is_transparent = Config.bIsTransparent;
	ViewCfg.initial_device_scale = Config.DeviceScale;
	ViewCfg.enable_javascript = Config.bEnableJavaScript;

	uint32 W = FMath::Max(static_cast<int32>(1), Config.Width);
	uint32 H = FMath::Max(static_cast<int32>(1), Config.Height);

	// Create the Ultralight View
	ultralight::RefPtr<ultralight::View> ULView = Renderer->CreateView(W, H, ViewCfg, nullptr);
	if (!ULView.get())
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateView: Ultralight failed to create view '%s'."), *ViewName.ToString());
		return NAME_None;
	}

	// Create the UE texture
	UTexture2D* Texture = UTexture2D::CreateTransient(W, H, PF_B8G8R8A8);
	if (!Texture)
	{
		UE_LOG(LogUltralightUE, Error, TEXT("CreateView: Failed to create UTexture2D for view '%s'."), *ViewName.ToString());
		return NAME_None;
	}

	Texture->SRGB = true;
	Texture->Filter = TF_Bilinear;
	Texture->AddToRoot(); // prevent GC
	Texture->UpdateResource();

	// Build entry
	TSharedPtr<FULUEViewEntry> Entry = MakeShared<FULUEViewEntry>();
	Entry->ViewName = ViewName;
	Entry->View = ULView;
	Entry->UETexture = Texture;
	Entry->bIsAccelerated = ViewCfg.is_accelerated;
	Entry->Width = W;
	Entry->Height = H;
	Entry->OwningSubsystem = this;

	// Register listeners
	ULView->set_view_listener(Entry.Get());
	ULView->set_load_listener(Entry.Get());

	Views.Add(ViewName, Entry);

	UE_LOG(LogUltralightUE, Log, TEXT("CreateView: Created view '%s' (%ux%u, accelerated=%s)."),
		*ViewName.ToString(), W, H, ViewCfg.is_accelerated ? TEXT("true") : TEXT("false"));

	return ViewName;
}

void UUltralightSubsystem::DestroyView(FName ViewName)
{
	TSharedPtr<FULUEViewEntry>* Found = Views.Find(ViewName);
	if (!Found)
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("DestroyView: View '%s' not found."), *ViewName.ToString());
		return;
	}

	FULUEViewEntry& Entry = **Found;

	// Unregister listeners
	if (Entry.View.get())
	{
		Entry.View->set_view_listener(nullptr);
		Entry.View->set_load_listener(nullptr);
	}

	// Release the UE texture
	if (Entry.UETexture)
	{
		Entry.UETexture->RemoveFromRoot();
		Entry.UETexture = nullptr;
	}

	// The RefPtr will release the Ultralight view when the shared pointer is destroyed
	Views.Remove(ViewName);

	UE_LOG(LogUltralightUE, Log, TEXT("DestroyView: Destroyed view '%s'."), *ViewName.ToString());
}

// ===========================================================================
// Navigation
// ===========================================================================

void UUltralightSubsystem::LoadURL(FName ViewName, const FString& URL)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("LoadURL: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->LoadURL(FStringToULString(URL));
	UE_LOG(LogUltralightUE, Log, TEXT("LoadURL: [%s] -> %s"), *ViewName.ToString(), *URL);
}

void UUltralightSubsystem::LoadHTML(FName ViewName, const FString& HTMLContent)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("LoadHTML: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->LoadHTML(FStringToULString(HTMLContent));
	UE_LOG(LogUltralightUE, Verbose, TEXT("LoadHTML: [%s] loaded HTML content."), *ViewName.ToString());
}

void UUltralightSubsystem::Reload(FName ViewName)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("Reload: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->Reload();
}

void UUltralightSubsystem::Stop(FName ViewName)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("Stop: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->Stop();
}

void UUltralightSubsystem::GoBack(FName ViewName)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("GoBack: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->GoBack();
}

void UUltralightSubsystem::GoForward(FName ViewName)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("GoForward: View '%s' not found."), *ViewName.ToString());
		return;
	}

	Entry->View->GoForward();
}

// ===========================================================================
// JavaScript
// ===========================================================================

FString UUltralightSubsystem::EvaluateJavaScript(FName ViewName, const FString& Script)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("EvaluateJavaScript: View '%s' not found."), *ViewName.ToString());
		return FString();
	}

	ultralight::String ExceptionStr;
	ultralight::String Result = Entry->View->EvaluateScript(FStringToULString(Script), &ExceptionStr);

	if (!ExceptionStr.empty())
	{
		FString ExFStr = ULStringToFString(ExceptionStr);
		UE_LOG(LogUltralightUE, Error, TEXT("EvaluateJavaScript: [%s] Exception: %s"), *ViewName.ToString(), *ExFStr);
		return ExFStr;
	}

	return ULStringToFString(Result);
}

// ===========================================================================
// Texture access
// ===========================================================================

UTexture2D* UUltralightSubsystem::GetViewTexture(FName ViewName) const
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry)
	{
		return nullptr;
	}
	return Entry->UETexture;
}

// ===========================================================================
// Input forwarding
// ===========================================================================

void UUltralightSubsystem::SetViewFocus(FName ViewName, bool bFocused)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	if (bFocused)
	{
		Entry->View->Focus();
	}
	else
	{
		Entry->View->Unfocus();
	}
}

void UUltralightSubsystem::FireMouseMoveEvent(FName ViewName, FVector2D Position)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	ultralight::MouseEvent Evt;
	Evt.type = ultralight::MouseEvent::kType_MouseMoved;
	Evt.x = static_cast<int>(Position.X);
	Evt.y = static_cast<int>(Position.Y);
	Evt.button = ultralight::MouseEvent::kButton_None;
	Entry->View->FireMouseEvent(Evt);
}

void UUltralightSubsystem::FireMouseButtonEvent(FName ViewName, FVector2D Position, int32 Button, bool bIsDown)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	ultralight::MouseEvent Evt;
	Evt.type = bIsDown ? ultralight::MouseEvent::kType_MouseDown : ultralight::MouseEvent::kType_MouseUp;
	Evt.x = static_cast<int>(Position.X);
	Evt.y = static_cast<int>(Position.Y);

	switch (Button)
	{
	case 0: Evt.button = ultralight::MouseEvent::kButton_Left; break;
	case 1: Evt.button = ultralight::MouseEvent::kButton_Middle; break;
	case 2: Evt.button = ultralight::MouseEvent::kButton_Right; break;
	default: Evt.button = ultralight::MouseEvent::kButton_None; break;
	}

	Entry->View->FireMouseEvent(Evt);
}

void UUltralightSubsystem::FireScrollEvent(FName ViewName, FVector2D Delta)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	ultralight::ScrollEvent Evt;
	Evt.type = ultralight::ScrollEvent::kType_ScrollByPixel;
	Evt.delta_x = static_cast<int>(Delta.X);
	Evt.delta_y = static_cast<int>(Delta.Y);
	Entry->View->FireScrollEvent(Evt);
}

void UUltralightSubsystem::FireKeyEvent(FName ViewName, int32 VirtualKeyCode, bool bIsDown)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	ultralight::KeyEvent Evt;
	Evt.type = bIsDown ? ultralight::KeyEvent::kType_RawKeyDown : ultralight::KeyEvent::kType_KeyUp;
	Evt.virtual_key_code = VirtualKeyCode;
	Evt.native_key_code = 0;
	Evt.modifiers = 0;
	Entry->View->FireKeyEvent(Evt);
}

void UUltralightSubsystem::FireCharEvent(FName ViewName, const FString& Character)
{
	FULUEViewEntry* Entry = FindViewEntry(ViewName);
	if (!Entry || !Entry->View.get())
	{
		return;
	}

	ultralight::KeyEvent Evt;
	Evt.type = ultralight::KeyEvent::kType_Char;
	Evt.text = FStringToULString(Character);
	Evt.unmodified_text = Evt.text;
	Evt.virtual_key_code = 0;
	Evt.native_key_code = 0;
	Evt.modifiers = 0;
	Entry->View->FireKeyEvent(Evt);
}

// ===========================================================================
// Static accessor
// ===========================================================================

UUltralightSubsystem* UUltralightSubsystem::GetUltralightSubsystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		return nullptr;
	}

	return GI->GetSubsystem<UUltralightSubsystem>();
}

// ===========================================================================
// Internal: platform setup / teardown
// ===========================================================================

void UUltralightSubsystem::SetupPlatform()
{
	// ---- Config ----
	ultralight::Config Cfg;
	Cfg.face_winding = ultralight::FaceWinding::Clockwise;

	FString CachePath = FPaths::ProjectSavedDir() / TEXT("UltralightCache");
	FTCHARToUTF8 CachePathUTF8(*CachePath);
	Cfg.cache_path = ultralight::String(CachePathUTF8.Get(), CachePathUTF8.Length());
	Cfg.resource_path_prefix = "resources/";

	// ---- FileSystem ----
	ultralightue::ULUEFileSystem* FS = new ultralightue::ULUEFileSystem();
	FS->SetFSAccess(ultralightue::FSAccess::FSA_Native);
	FileSystemPtr = FS;

	// ---- FontLoader ----
	ultralightue::ULUEFontLoader* FL = new ultralightue::ULUEFontLoader();
	FontLoaderPtr = FL;

	// ---- Clipboard ----
	ultralightue::ULUEClipboard* CB = new ultralightue::ULUEClipboard();
	ClipboardPtr = CB;

	// ---- Register with Platform ----
	ultralight::Platform& Platform = ultralight::Platform::instance();
	Platform.set_config(Cfg);
	Platform.set_file_system(FS);
	Platform.set_font_loader(FL);
	Platform.set_clipboard(CB);

	// ---- Create Renderer ----
	RendererPtr = new ultralight::RefPtr<ultralight::Renderer>(ultralight::Renderer::Create());

	UE_LOG(LogUltralightUE, Log, TEXT("SetupPlatform: Ultralight renderer created (cache: %s)."), *CachePath);
}

void UUltralightSubsystem::TeardownPlatform()
{
	// Release renderer
	if (RendererPtr)
	{
		delete static_cast<ultralight::RefPtr<ultralight::Renderer>*>(RendererPtr);
		RendererPtr = nullptr;
	}

	// Release platform objects
	if (FileSystemPtr)
	{
		delete static_cast<ultralightue::ULUEFileSystem*>(FileSystemPtr);
		FileSystemPtr = nullptr;
	}

	if (FontLoaderPtr)
	{
		delete static_cast<ultralightue::ULUEFontLoader*>(FontLoaderPtr);
		FontLoaderPtr = nullptr;
	}

	if (ClipboardPtr)
	{
		delete static_cast<ultralightue::ULUEClipboard*>(ClipboardPtr);
		ClipboardPtr = nullptr;
	}

	UE_LOG(LogUltralightUE, Log, TEXT("TeardownPlatform: Ultralight platform released."));
}

// ===========================================================================
// Internal: CPU texture update
// ===========================================================================

void UUltralightSubsystem::UpdateCPUTexture(FULUEViewEntry& Entry)
{
	if (!Entry.View.get() || !Entry.UETexture)
	{
		return;
	}

	ultralight::Surface* SurfaceRaw = Entry.View->surface();
	if (!SurfaceRaw)
	{
		return;
	}

	ultralight::BitmapSurface* Surface = static_cast<ultralight::BitmapSurface*>(SurfaceRaw);
	ultralight::IntRect DirtyBounds = Surface->dirty_bounds();

	// Nothing to update if there are no dirty pixels
	if (DirtyBounds.IsEmpty())
	{
		return;
	}

	ultralight::RefPtr<ultralight::Bitmap> Bitmap = Surface->bitmap();
	if (!Bitmap.get())
	{
		return;
	}

	void* Pixels = Bitmap->LockPixels();
	if (!Pixels)
	{
		Bitmap->UnlockPixels();
		return;
	}

	const uint32 SrcRowBytes = Bitmap->row_bytes();
	const uint32 SrcWidth = Bitmap->width();
	const uint32 SrcHeight = Bitmap->height();
	const uint32 BytesPerPixel = Bitmap->bpp();

	// Update the full texture mip data
	FTexture2DMipMap& Mip = Entry.UETexture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);

	if (TextureData)
	{
		const uint32 DstRowBytes = SrcWidth * BytesPerPixel;

		for (uint32 Row = 0; Row < SrcHeight; ++Row)
		{
			const uint8* SrcRow = static_cast<const uint8*>(Pixels) + (Row * SrcRowBytes);
			uint8* DstRow = static_cast<uint8*>(TextureData) + (Row * DstRowBytes);
			FMemory::Memcpy(DstRow, SrcRow, DstRowBytes);
		}
	}

	Mip.BulkData.Unlock();
	Bitmap->UnlockPixels();

	Entry.UETexture->UpdateResource();
	Surface->ClearDirtyBounds();
}

// ===========================================================================
// Internal: find view entry
// ===========================================================================

FULUEViewEntry* UUltralightSubsystem::FindViewEntry(FName ViewName) const
{
	const TSharedPtr<FULUEViewEntry>* Found = Views.Find(ViewName);
	if (!Found || !Found->IsValid())
	{
		return nullptr;
	}
	return Found->Get();
}
