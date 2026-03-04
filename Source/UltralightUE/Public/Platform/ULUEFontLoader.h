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

namespace ultralightue
{
	/**
	 * Unreal Engine implementation of Ultralight's FontLoader interface.
	 *
	 * Scans system font directories for .ttf/.otf/.ttc files and provides
	 * them to the Ultralight renderer on demand. Builds a lazily-initialised
	 * lookup map of lowercased family name -> file path.
	 */
	class ULTRALIGHTUE_API ULUEFontLoader : public ultralight::FontLoader
	{
	public:
		ULUEFontLoader();
		virtual ~ULUEFontLoader() override;

		// -------------------------------------------------------------------
		// ultralight::FontLoader interface
		// -------------------------------------------------------------------

		/** Returns the platform-appropriate fallback font family name. */
		virtual ultralight::String fallback_font() const override;

		/**
		 * Returns an appropriate fallback font for a set of characters,
		 * with CJK-aware heuristics.
		 */
		virtual ultralight::String fallback_font_for_characters(
			const ultralight::String& characters,
			int weight,
			bool italic) const override;

		/**
		 * Load a font file by family name, weight, and italic flag.
		 * Returns a FontFile wrapping the on-disk font, or nullptr on failure.
		 */
		virtual ultralight::RefPtr<ultralight::FontFile> Load(
			const ultralight::String& family,
			int weight,
			bool italic) override;

	private:
		/** Lazily populate the FontMap from system font directories. */
		void EnsureFontMapInitialised();

		/**
		 * Map of lowercased font family name (derived from filename) to
		 * the full file path.  Built once on first Load() call.
		 */
		TMap<FString, FString> FontMap;

		/** True once FontMap has been populated. */
		bool bFontMapInitialised = false;
	};
}
