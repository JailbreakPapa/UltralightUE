#include "Assets/ULUEFontAsset.h"

FString UULUEFontAsset::GetVirtualFilePath() const
{
	FString StyleSuffix = bIsItalic ? TEXT("_Italic") : TEXT("");
	return FString::Printf(TEXT("/Fonts/%s_%d%s.ttf"), *FamilyName, Weight, *StyleSuffix);
}
