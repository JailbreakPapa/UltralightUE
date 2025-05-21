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
#include <Ultralight/platform/FontLoader.h>
#include "Assets/ULUEFontAsset.h"

class UULUEFontAsset;

class ULTRALIGHTUE_API ULUEFontSystem : public ultralight::FontLoader
{
public:
	ULUEFontSystem(const TArray<UULUEFontAsset*>& InFonts, const FString& InFallbackFontFamily)
		: Fonts(InFonts), FallbackFontFamily(InFallbackFontFamily)
	{
	}

	virtual ~ULUEFontSystem() override
	{
	}

	// Returns the fallback font family name
	virtual ultralight::String fallback_font() const override;

	// Finds a font family that can render the specified characters
	virtual ultralight::String fallback_font_for_characters(const ultralight::String& characters, int weight,
	                                                        bool italic) const override;

	// Loads font data for the specified font description
	virtual ultralight::RefPtr<ultralight::FontFile>
	Load(const ultralight::String& family, int weight, bool italic) override;

private:
	// Helper function to check if a font supports all given characters
	bool FontSupportsCharacters(const UULUEFontAsset* Font, const ultralight::String& characters) const;
	TArray<UULUEFontAsset*> Fonts;
	FString FallbackFontFamily;
};
