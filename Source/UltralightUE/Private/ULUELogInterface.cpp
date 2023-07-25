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
#include "UltralightUE/Public/ULUELogInterface.h"

 /// Include FString, due to forward declaration.
#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"

void ultralightue::ULUELogInterface::LogError(FString& details)
{
	UE_LOG(LogTemp, Error, TEXT("UltralightUE Error: %s"), *details);
}

void ultralightue::ULUELogInterface::LogWarning(FString& details)
{
	UE_LOG(LogTemp, Warning, TEXT("UltralightUE Warning: %s"), *details);
}

void ultralightue::ULUELogInterface::LogInfo(FString& details)
{
	UE_LOG(LogTemp, Log, TEXT("UltralightUE: %s"), *details);
}