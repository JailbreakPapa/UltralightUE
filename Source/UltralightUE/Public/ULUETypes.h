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
#include "ULUETypes.generated.h"

UENUM(BlueprintType)
enum class EUltralightRenderMode : uint8
{
	CPU    UMETA(DisplayName = "CPU (Default)"),
	GPU    UMETA(DisplayName = "GPU Accelerated"),
};

USTRUCT(BlueprintType)
struct ULTRALIGHTUE_API FUltralightViewConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	int32 Width = 1280;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	int32 Height = 720;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	bool bIsTransparent = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	bool bEnableJavaScript = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	float DeviceScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultralight")
	EUltralightRenderMode RenderMode = EUltralightRenderMode::CPU;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUltralightDOMReady, FName, ViewName, const FString&, URL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUltralightLoadFinished, FName, ViewName, const FString&, URL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnUltralightConsoleMessage, FName, ViewName, const FString&, Message, const FString&, Source, int32, Line);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUltralightLoadFailed, FName, ViewName, const FString&, URL, const FString&, Description);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJSFunctionCalled, const FString&, FunctionName, const FString&, ArgsJSON);
