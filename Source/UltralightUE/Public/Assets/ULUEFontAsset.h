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
#include "ULUEFontAsset.generated.h"

/**
 * @brief Font asset for using Fonts with the plugin.
 * @warning ALWAYS create a font asset for a font you want to use in HTML/CSS. Otherwise, missing font glyphs will pop up on packaging!
 */
UCLASS()
class ULTRALIGHTUE_API UULUEFontAsset : public UObject
{
	GENERATED_BODY()

public:
	// Font family name (e.g., "Arial")
	UPROPERTY(EditAnywhere, Category = "Font")
	FString FamilyName;

	// Font weight (e.g., 400 for normal, 700 for bold)
	UPROPERTY(EditAnywhere, Category = "Font")
	int32 Weight = 400;

	// Whether the font is italic
	UPROPERTY(EditAnywhere, Category = "Font")
	bool bIsItalic = false;

	// Raw font data (TTF or OTF bytes)
	UPROPERTY(Category = "Font")
	TArray<uint8> FontData;

	UPROPERTY(EditAnywhere, Category = "Font")
	UFont* FontAsset;

	// Get the virtual file path for this font (e.g., "/Fonts/Arial_400.ttf")
	FString GetVirtualFilePath() const;
};
