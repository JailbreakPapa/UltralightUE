#include "ULUEFontSystem.h"
#include "Engine/Font.h"

ultralight::String ULUEFontSystem::fallback_font() const
{
	return ultralight::String(TCHAR_TO_UTF8(*FallbackFontFamily));
}

ultralight::String ULUEFontSystem::fallback_font_for_characters(const ultralight::String& characters, int weight,
                                                                bool italic) const
{
	UULUEFontAsset* BestFont = nullptr;
	int BestScore = INT_MAX;

	for (UULUEFontAsset* Font : Fonts)
	{
		if (FontSupportsCharacters(Font, characters))
		{
			int WeightDiff = FMath::Abs(Font->Weight - weight);
			bool ItalicMatch = Font->bIsItalic == italic;
			int Score = WeightDiff * 2 + (ItalicMatch ? 0 : 1); // Weight difference has higher priority
			if (Score < BestScore)
			{
				BestScore = Score;
				BestFont = Font;
			}
		}
	}

	if (BestFont)
	{
		return ultralight::String(TCHAR_TO_UTF8(*BestFont->FamilyName));
	}
	return fallback_font();
}

ultralight::RefPtr<ultralight::FontFile> ULUEFontSystem::Load(const ultralight::String& family, int weight, bool italic)
{
	FString FamilyStr = FString(family.utf8().data());
	UULUEFontAsset* BestFont = nullptr;
	int BestScore = INT_MAX;

	for (UULUEFontAsset* Font : Fonts)
	{
		if (Font->FamilyName == FamilyStr)
		{
			int WeightDiff = FMath::Abs(Font->Weight - weight);
			bool ItalicMatch = Font->bIsItalic == italic;
			int Score = WeightDiff * 2 + (ItalicMatch ? 0 : 1);
			if (Score < BestScore)
			{
				BestScore = Score;
				BestFont = Font;
			}
		}
	}

	if (BestFont && BestFont->FontAsset)
	{
		const FCompositeFont* CompositeFont = BestFont->FontAsset->GetCompositeFont();
		if (CompositeFont && CompositeFont->DefaultTypeface.Fonts.Num() > 0)
		{
			const FFontData& FontDataObj = CompositeFont->DefaultTypeface.Fonts[0].Font;
			TArray<uint8> FontData;
			if (auto dad = FontDataObj.GetFontFaceData())
			{
				FontData = dad.Get()->GetData();
				if (FontData.Num() > 0)
				{
					ultralight::RefPtr<ultralight::Buffer> Buffer = ultralight::Buffer::CreateFromCopy(
						FontData.GetData(), FontData.Num());
					return ultralight::FontFile::Create(Buffer);
				}
			}
		}
	}
	return nullptr; // Fallback to Ultralight's default handling
}

bool ULUEFontSystem::FontSupportsCharacters(const UULUEFontAsset* Font, const ultralight::String& characters) const
{
	// TODO: I Dont even know.
	/*for (size_t i = 0; i < characters.utf8().length(); ++i)
	{
		int32 CodePoint = characters.utf8()[i]; // UTF-16 code point
		bool Supported = false;
		for (const FInt32Range& Range : Font->SupportedCodePointRanges)
		{
			if (CodePoint >= Range.GetMin() && CodePoint <= Range.GetMax())
			{
				Supported = true;
				break;
			}
		}
		if (!Supported)
		{
			return false;
		}
	}*/
	return true;
}
