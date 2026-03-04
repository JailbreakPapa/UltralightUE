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

#include "ULUEViewEntry.h"
#include "ULUESubsystem.h"
#include "ULUELogging.h"

// ---------------------------------------------------------------------------
// Helper: convert ultralight::String -> FString
// ---------------------------------------------------------------------------
static FString ULStringToFString(const ultralight::String& InString)
{
	return FString(UTF8_TO_TCHAR(InString.utf8().data()));
}

// ===========================================================================
// ViewListener overrides
// ===========================================================================

void FULUEViewEntry::OnChangeTitle(ultralight::View* caller, const ultralight::String& title)
{
	FString TitleStr = ULStringToFString(title);
	UE_LOG(LogUltralightUE, Verbose, TEXT("[%s] Title changed: %s"), *ViewName.ToString(), *TitleStr);
}

void FULUEViewEntry::OnChangeURL(ultralight::View* caller, const ultralight::String& url)
{
	FString URLStr = ULStringToFString(url);
	UE_LOG(LogUltralightUE, Verbose, TEXT("[%s] URL changed: %s"), *ViewName.ToString(), *URLStr);
}

void FULUEViewEntry::OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor)
{
	UE_LOG(LogUltralightUE, Verbose, TEXT("[%s] Cursor changed to: %d"), *ViewName.ToString(), static_cast<int32>(cursor));
}

void FULUEViewEntry::OnAddConsoleMessage(ultralight::View* caller,
	ultralight::MessageSource source,
	ultralight::MessageLevel level,
	const ultralight::String& message,
	uint32_t line_number,
	uint32_t column_number,
	const ultralight::String& source_id)
{
	FString MessageStr = ULStringToFString(message);
	FString SourceStr = ULStringToFString(source_id);

	switch (level)
	{
	case ultralight::kMessageLevel_Log:
		UE_LOG(LogUltralightUE, Log, TEXT("[%s] Console: %s (source: %s, line: %u)"),
			*ViewName.ToString(), *MessageStr, *SourceStr, line_number);
		break;
	case ultralight::kMessageLevel_Warning:
		UE_LOG(LogUltralightUE, Warning, TEXT("[%s] Console Warning: %s (source: %s, line: %u)"),
			*ViewName.ToString(), *MessageStr, *SourceStr, line_number);
		break;
	case ultralight::kMessageLevel_Error:
		UE_LOG(LogUltralightUE, Error, TEXT("[%s] Console Error: %s (source: %s, line: %u)"),
			*ViewName.ToString(), *MessageStr, *SourceStr, line_number);
		break;
	default:
		UE_LOG(LogUltralightUE, Verbose, TEXT("[%s] Console (%d): %s (source: %s, line: %u)"),
			*ViewName.ToString(), static_cast<int32>(level), *MessageStr, *SourceStr, line_number);
		break;
	}

	if (OwningSubsystem)
	{
		OwningSubsystem->OnConsoleMessage.Broadcast(ViewName, MessageStr, SourceStr, static_cast<int32>(line_number));
	}
}

// ===========================================================================
// LoadListener overrides
// ===========================================================================

void FULUEViewEntry::OnBeginLoading(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const ultralight::String& url)
{
	FString URLStr = ULStringToFString(url);
	UE_LOG(LogUltralightUE, Log, TEXT("[%s] Begin loading: %s (frame: %llu, main: %s)"),
		*ViewName.ToString(), *URLStr, frame_id, is_main_frame ? TEXT("true") : TEXT("false"));
}

void FULUEViewEntry::OnFinishLoading(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const ultralight::String& url)
{
	FString URLStr = ULStringToFString(url);
	UE_LOG(LogUltralightUE, Log, TEXT("[%s] Finished loading: %s (frame: %llu, main: %s)"),
		*ViewName.ToString(), *URLStr, frame_id, is_main_frame ? TEXT("true") : TEXT("false"));

	if (OwningSubsystem && is_main_frame)
	{
		OwningSubsystem->OnLoadFinished.Broadcast(ViewName, URLStr);
	}
}

void FULUEViewEntry::OnFailLoading(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const ultralight::String& url,
	const ultralight::String& description,
	const ultralight::String& error_domain,
	int error_code)
{
	FString URLStr = ULStringToFString(url);
	FString DescStr = ULStringToFString(description);
	FString DomainStr = ULStringToFString(error_domain);

	UE_LOG(LogUltralightUE, Error, TEXT("[%s] Failed loading: %s - %s (domain: %s, code: %d)"),
		*ViewName.ToString(), *URLStr, *DescStr, *DomainStr, error_code);

	if (OwningSubsystem && is_main_frame)
	{
		OwningSubsystem->OnLoadFailed.Broadcast(ViewName, URLStr, DescStr);
	}
}

void FULUEViewEntry::OnDOMReady(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const ultralight::String& url)
{
	FString URLStr = ULStringToFString(url);
	UE_LOG(LogUltralightUE, Log, TEXT("[%s] DOM ready: %s (frame: %llu, main: %s)"),
		*ViewName.ToString(), *URLStr, frame_id, is_main_frame ? TEXT("true") : TEXT("false"));

	if (OwningSubsystem && is_main_frame)
	{
		OwningSubsystem->OnDOMReady.Broadcast(ViewName, URLStr);
	}
}

void FULUEViewEntry::OnWindowObjectReady(ultralight::View* caller,
	uint64_t frame_id,
	bool is_main_frame,
	const ultralight::String& url)
{
	FString URLStr = ULStringToFString(url);
	UE_LOG(LogUltralightUE, Verbose, TEXT("[%s] Window object ready: %s (frame: %llu, main: %s)"),
		*ViewName.ToString(), *URLStr, frame_id, is_main_frame ? TEXT("true") : TEXT("false"));
}
