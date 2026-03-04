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
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "ULUETypes.h"
#include "ULUESubsystem.generated.h"

// Forward declarations - no Ultralight SDK headers in the public header
struct FULUEViewEntry;
class UTexture2D;

namespace ultralightue
{
	class ULUEFileSystem;
	class ULUEFontLoader;
	class ULUEClipboard;
}

/**
 * UUltralightSubsystem
 *
 * A UGameInstanceSubsystem that manages the Ultralight renderer, views, and
 * platform integration. Ticks via FTickableGameObject to drive the Ultralight
 * update/render loop and copy CPU-rendered bitmaps into UTexture2D assets.
 */
UCLASS()
class ULTRALIGHTUE_API UUltralightSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:

	// -----------------------------------------------------------------------
	// USubsystem interface
	// -----------------------------------------------------------------------

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// -----------------------------------------------------------------------
	// FTickableGameObject interface
	// -----------------------------------------------------------------------

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return bIsInitialized; }
	virtual bool IsTickableInEditor() const override { return false; }

	// -----------------------------------------------------------------------
	// View management
	// -----------------------------------------------------------------------

	/** Create a new Ultralight view with the given name and configuration. Returns the view name. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Views")
	FName CreateView(FName ViewName, const FUltralightViewConfig& Config);

	/** Destroy a previously created view and release its resources. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Views")
	void DestroyView(FName ViewName);

	// -----------------------------------------------------------------------
	// Navigation
	// -----------------------------------------------------------------------

	/** Load a URL in the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void LoadURL(FName ViewName, const FString& URL);

	/** Load raw HTML content in the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void LoadHTML(FName ViewName, const FString& HTMLContent);

	/** Reload the current page in the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void Reload(FName ViewName);

	/** Stop loading the current page in the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void Stop(FName ViewName);

	/** Navigate backwards in history. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void GoBack(FName ViewName);

	/** Navigate forwards in history. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Navigation")
	void GoForward(FName ViewName);

	// -----------------------------------------------------------------------
	// JavaScript
	// -----------------------------------------------------------------------

	/** Evaluate a JavaScript expression in the specified view and return the result as a string. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|JavaScript")
	FString EvaluateJavaScript(FName ViewName, const FString& Script);

	// -----------------------------------------------------------------------
	// Texture access
	// -----------------------------------------------------------------------

	/** Get the UTexture2D associated with the specified view (CPU render mode). */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Rendering")
	UTexture2D* GetViewTexture(FName ViewName) const;

	// -----------------------------------------------------------------------
	// Input forwarding
	// -----------------------------------------------------------------------

	/** Set keyboard focus on the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void SetViewFocus(FName ViewName, bool bFocused);

	/** Forward a mouse move event to the specified view. Position is in view-local pixels. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void FireMouseMoveEvent(FName ViewName, FVector2D Position);

	/** Forward a mouse button event to the specified view. Button: 0=Left, 1=Middle, 2=Right. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void FireMouseButtonEvent(FName ViewName, FVector2D Position, int32 Button, bool bIsDown);

	/** Forward a mouse scroll event to the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void FireScrollEvent(FName ViewName, FVector2D Delta);

	/** Forward a key event to the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void FireKeyEvent(FName ViewName, int32 VirtualKeyCode, bool bIsDown);

	/** Forward a character input event to the specified view. */
	UFUNCTION(BlueprintCallable, Category = "UltralightUE|Input")
	void FireCharEvent(FName ViewName, const FString& Character);

	// -----------------------------------------------------------------------
	// Static accessor
	// -----------------------------------------------------------------------

	/** Convenience static method to retrieve the subsystem from any world context object. */
	UFUNCTION(BlueprintPure, Category = "UltralightUE", meta = (WorldContext = "WorldContextObject"))
	static UUltralightSubsystem* GetUltralightSubsystem(UObject* WorldContextObject);

	// -----------------------------------------------------------------------
	// Blueprint delegates
	// -----------------------------------------------------------------------

	UPROPERTY(BlueprintAssignable, Category = "UltralightUE|Events")
	FOnUltralightDOMReady OnDOMReady;

	UPROPERTY(BlueprintAssignable, Category = "UltralightUE|Events")
	FOnUltralightLoadFinished OnLoadFinished;

	UPROPERTY(BlueprintAssignable, Category = "UltralightUE|Events")
	FOnUltralightConsoleMessage OnConsoleMessage;

	UPROPERTY(BlueprintAssignable, Category = "UltralightUE|Events")
	FOnUltralightLoadFailed OnLoadFailed;

private:

	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------

	/** Configure and register Ultralight platform singletons (FileSystem, FontLoader, etc.) */
	void SetupPlatform();

	/** Tear down and unregister Ultralight platform singletons. */
	void TeardownPlatform();

	/** Copy the CPU-rendered bitmap of a view into its associated UTexture2D. */
	void UpdateCPUTexture(FULUEViewEntry& Entry);

	/** Find a view entry by name. Returns nullptr if not found. */
	FULUEViewEntry* FindViewEntry(FName ViewName) const;

	// -----------------------------------------------------------------------
	// Data
	// -----------------------------------------------------------------------

	/** Map of active views keyed by their FName identifier. */
	TMap<FName, TSharedPtr<FULUEViewEntry>> Views;

	/**
	 * Opaque pointer to ultralight::RefPtr<ultralight::Renderer>.
	 * Heap-allocated to avoid pulling Ultralight headers into this public header.
	 */
	void* RendererPtr = nullptr;

	/** Platform implementations -- heap-allocated, opaque in the header. */
	void* FileSystemPtr = nullptr;
	void* FontLoaderPtr = nullptr;
	void* ClipboardPtr = nullptr;

	bool bIsInitialized = false;
};
