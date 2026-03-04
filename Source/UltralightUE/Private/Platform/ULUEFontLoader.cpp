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

#include "Platform/ULUEFontLoader.h"
#include "ULUELogging.h"

#include <Ultralight/platform/FontLoader.h>

#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

namespace ultralightue
{

// ---------------------------------------------------------------------------
// Helper: convert FString -> ultralight::String (UTF-8)
// ---------------------------------------------------------------------------
static ultralight::String FStringToUL(const FString& InString)
{
	FTCHARToUTF8 Converter(*InString);
	return ultralight::String(Converter.Get(), Converter.Length());
}

// ---------------------------------------------------------------------------
// Helper: convert ultralight::String -> FString
// ---------------------------------------------------------------------------
static FString ULToFString(const ultralight::String& InString)
{
	return FString(UTF8_TO_TCHAR(InString.utf8().data()));
}

// ---------------------------------------------------------------------------
// Helper: strip common font-name suffixes from a filename to produce
// a heuristic family name.
// e.g.  "arialbd.ttf" -> "arial",  "DejaVuSans-Bold.ttf" -> "dejavusans"
// ---------------------------------------------------------------------------
static FString DeriveFamily(const FString& Filename)
{
	// Start from the base name without extension
	FString Base = FPaths::GetBaseFilename(Filename).ToLower();

	// Remove common weight / style suffixes
	static const TCHAR* Suffixes[] = {
		TEXT("-bold"), TEXT("-italic"), TEXT("-bolditalic"),
		TEXT("-light"), TEXT("-medium"), TEXT("-regular"),
		TEXT("-thin"), TEXT("-black"), TEXT("-semibold"),
		TEXT("bold"), TEXT("italic"), TEXT("bd"), TEXT("it"),
		TEXT("bi"), TEXT("light"), TEXT("med"), TEXT("rg"),
	};

	for (const TCHAR* Suf : Suffixes)
	{
		if (Base.EndsWith(Suf))
		{
			Base.LeftChopInline(FCString::Strlen(Suf));
			break; // Only strip one suffix
		}
	}

	// Remove remaining dashes, underscores, and spaces so that
	// "DejaVu Sans" matches "dejavusans"
	Base.ReplaceInline(TEXT("-"), TEXT(""));
	Base.ReplaceInline(TEXT("_"), TEXT(""));
	Base.ReplaceInline(TEXT(" "), TEXT(""));

	return Base;
}

// ---------------------------------------------------------------------------
// Helper: detect if a string contains CJK codepoints
// ---------------------------------------------------------------------------
static bool ContainsCJK(const FString& Text)
{
	for (int32 i = 0; i < Text.Len(); ++i)
	{
		TCHAR Ch = Text[i];
		// CJK Unified Ideographs (U+4E00..U+9FFF)
		if (Ch >= 0x4E00 && Ch <= 0x9FFF) return true;
		// Hiragana (U+3040..U+309F)
		if (Ch >= 0x3040 && Ch <= 0x309F) return true;
		// Katakana (U+30A0..U+30FF)
		if (Ch >= 0x30A0 && Ch <= 0x30FF) return true;
		// Hangul Syllables (U+AC00..U+D7AF)
		if (Ch >= 0xAC00 && Ch <= 0xD7AF) return true;
	}
	return false;
}

// ===========================================================================
// Construction / destruction
// ===========================================================================

ULUEFontLoader::ULUEFontLoader()
{
}

ULUEFontLoader::~ULUEFontLoader()
{
}

// ===========================================================================
// ultralight::FontLoader interface
// ===========================================================================

ultralight::String ULUEFontLoader::fallback_font() const
{
#if PLATFORM_WINDOWS
	return "Arial";
#elif PLATFORM_MAC
	return "Helvetica";
#else
	return "DejaVu Sans";
#endif
}

ultralight::String ULUEFontLoader::fallback_font_for_characters(
	const ultralight::String& characters,
	int weight,
	bool italic) const
{
	FString Chars = ULToFString(characters);

	if (ContainsCJK(Chars))
	{
#if PLATFORM_WINDOWS
		// Microsoft YaHei covers Simplified Chinese and is widely available
		return "Microsoft YaHei";
#elif PLATFORM_MAC
		return "PingFang SC";
#else
		return "Noto Sans CJK SC";
#endif
	}

	return fallback_font();
}

ultralight::RefPtr<ultralight::FontFile> ULUEFontLoader::Load(
	const ultralight::String& family,
	int weight,
	bool italic)
{
	// Make sure the font directory has been scanned
	const_cast<ULUEFontLoader*>(this)->EnsureFontMapInitialised();

	FString FamilyStr = ULToFString(family).ToLower();

	// Normalise: strip spaces, dashes, underscores for matching
	FString NormFamily = FamilyStr;
	NormFamily.ReplaceInline(TEXT(" "), TEXT(""));
	NormFamily.ReplaceInline(TEXT("-"), TEXT(""));
	NormFamily.ReplaceInline(TEXT("_"), TEXT(""));

	// Direct lookup
	if (const FString* FoundPath = FontMap.Find(NormFamily))
	{
		UE_LOG(LogUltralightUE, Verbose, TEXT("FontLoader::Load: found '%s' -> %s"), *FamilyStr, **FoundPath);
		FTCHARToUTF8 PathUTF8(**FoundPath);
		return ultralight::FontFile::Create(ultralight::String(PathUTF8.Get(), PathUTF8.Length()));
	}

	// Fuzzy: try to see if any key starts with our normalised family
	for (const auto& Pair : FontMap)
	{
		if (Pair.Key.StartsWith(NormFamily))
		{
			UE_LOG(LogUltralightUE, Verbose, TEXT("FontLoader::Load: fuzzy match '%s' -> %s"), *FamilyStr, *Pair.Value);
			FTCHARToUTF8 PathUTF8(*Pair.Value);
			return ultralight::FontFile::Create(ultralight::String(PathUTF8.Get(), PathUTF8.Length()));
		}
	}

	// Fall back to the default font
	if (FamilyStr != ULToFString(fallback_font()).ToLower())
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("FontLoader::Load: '%s' not found, trying fallback '%s'."),
			*FamilyStr, *ULToFString(fallback_font()));
		return Load(fallback_font(), weight, italic);
	}

	UE_LOG(LogUltralightUE, Error, TEXT("FontLoader::Load: could not locate any font for '%s'."), *FamilyStr);
	return nullptr;
}

// ===========================================================================
// Internal
// ===========================================================================

void ULUEFontLoader::EnsureFontMapInitialised()
{
	if (bFontMapInitialised)
	{
		return;
	}
	bFontMapInitialised = true;

	UE_LOG(LogUltralightUE, Log, TEXT("FontLoader: Scanning system font directories..."));

	// Collect font directories per platform
	TArray<FString> FontDirs;

#if PLATFORM_WINDOWS
	FontDirs.Add(TEXT("C:/Windows/Fonts"));

	// Also check per-user fonts on Windows 10+
	FString LocalAppData = FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA"));
	if (!LocalAppData.IsEmpty())
	{
		FString UserFonts = LocalAppData / TEXT("Microsoft/Windows/Fonts");
		FontDirs.Add(UserFonts);
	}
#elif PLATFORM_MAC
	FontDirs.Add(TEXT("/System/Library/Fonts"));
	FontDirs.Add(TEXT("/Library/Fonts"));

	FString HomeDir = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));
	if (!HomeDir.IsEmpty())
	{
		FontDirs.Add(HomeDir / TEXT("Library/Fonts"));
	}
#elif PLATFORM_LINUX
	FontDirs.Add(TEXT("/usr/share/fonts"));
	FontDirs.Add(TEXT("/usr/local/share/fonts"));

	FString HomeDir = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));
	if (!HomeDir.IsEmpty())
	{
		FontDirs.Add(HomeDir / TEXT(".local/share/fonts"));
		FontDirs.Add(HomeDir / TEXT(".fonts"));
	}
#endif

	// Scan each directory for font files
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	for (const FString& Dir : FontDirs)
	{
		if (!PlatformFile.DirectoryExists(*Dir))
		{
			continue;
		}

		// Visitor that records .ttf / .otf / .ttc files
		struct FFontVisitor : public IPlatformFile::FDirectoryVisitor
		{
			TMap<FString, FString>& OutMap;
			FFontVisitor(TMap<FString, FString>& InMap) : OutMap(InMap) {}

			virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
			{
				if (bIsDirectory)
				{
					return true; // continue traversal
				}

				FString Path(FilenameOrDirectory);
				FString Ext = FPaths::GetExtension(Path).ToLower();

				if (Ext == TEXT("ttf") || Ext == TEXT("otf") || Ext == TEXT("ttc"))
				{
					FString Family = DeriveFamily(FPaths::GetCleanFilename(Path));
					if (!Family.IsEmpty() && !OutMap.Contains(Family))
					{
						OutMap.Add(Family, Path);
					}
				}

				return true; // continue
			}
		};

		FFontVisitor Visitor(FontMap);
		PlatformFile.IterateDirectoryRecursively(*Dir, Visitor);
	}

	UE_LOG(LogUltralightUE, Log, TEXT("FontLoader: Indexed %d font families."), FontMap.Num());
}

} // namespace ultralightue
