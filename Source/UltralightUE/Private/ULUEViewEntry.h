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
#include <Ultralight/Ultralight.h>
#include <Ultralight/Listener.h>

class UUltralightSubsystem;
class UTexture2D;

/**
 * Internal struct wrapping a single Ultralight View.
 * Non-UObject. Implements ultralight::ViewListener and ultralight::LoadListener
 * to receive callbacks from the Ultralight rendering engine.
 */
struct FULUEViewEntry : public ultralight::ViewListener, public ultralight::LoadListener
{
	FName ViewName;
	ultralight::RefPtr<ultralight::View> View;
	UTexture2D* UETexture = nullptr;
	bool bIsAccelerated = false;
	uint32 Width = 0;
	uint32 Height = 0;

	/** Back-pointer to the owning subsystem for delegate broadcasting */
	UUltralightSubsystem* OwningSubsystem = nullptr;

	// -----------------------------------------------------------------------
	// ViewListener overrides
	// -----------------------------------------------------------------------

	virtual void OnChangeTitle(ultralight::View* caller,
		const ultralight::String& title) override;

	virtual void OnChangeURL(ultralight::View* caller,
		const ultralight::String& url) override;

	virtual void OnChangeCursor(ultralight::View* caller,
		ultralight::Cursor cursor) override;

	virtual void OnAddConsoleMessage(ultralight::View* caller,
		ultralight::MessageSource source,
		ultralight::MessageLevel level,
		const ultralight::String& message,
		uint32_t line_number,
		uint32_t column_number,
		const ultralight::String& source_id) override;

	// -----------------------------------------------------------------------
	// LoadListener overrides
	// -----------------------------------------------------------------------

	virtual void OnBeginLoading(ultralight::View* caller,
		uint64_t frame_id,
		bool is_main_frame,
		const ultralight::String& url) override;

	virtual void OnFinishLoading(ultralight::View* caller,
		uint64_t frame_id,
		bool is_main_frame,
		const ultralight::String& url) override;

	virtual void OnFailLoading(ultralight::View* caller,
		uint64_t frame_id,
		bool is_main_frame,
		const ultralight::String& url,
		const ultralight::String& description,
		const ultralight::String& error_domain,
		int error_code) override;

	virtual void OnDOMReady(ultralight::View* caller,
		uint64_t frame_id,
		bool is_main_frame,
		const ultralight::String& url) override;

	virtual void OnWindowObjectReady(ultralight::View* caller,
		uint64_t frame_id,
		bool is_main_frame,
		const ultralight::String& url) override;
};
