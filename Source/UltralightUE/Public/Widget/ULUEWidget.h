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
#include "Components/Widget.h"
#include "ULUETypes.h"
#include "ULUEWidget.generated.h"

class UUltralightSubsystem;
class SUltralightWidget;

UCLASS()
class ULTRALIGHTUE_API UUltralightWidget : public UWidget
{
	GENERATED_BODY()

public:

	// -----------------------------------------------------------------------
	// Configuration
	// -----------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltralightUE")
	FUltralightViewConfig ViewConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltralightUE")
	FName ViewName = TEXT("DefaultView");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UltralightUE")
	FString InitialURL;

	// -----------------------------------------------------------------------
	// Blueprint API
	// -----------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	void LoadURL(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	void LoadHTML(const FString& HTMLContent);

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	FString ExecuteJavaScript(const FString& Script);

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	void GoBack();

	UFUNCTION(BlueprintCallable, Category = "UltralightUE")
	void GoForward();

	// -----------------------------------------------------------------------
	// UWidget interface
	// -----------------------------------------------------------------------

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<SUltralightWidget> SlateWidget;
	bool bViewCreated = false;

	UUltralightSubsystem* GetSubsystem() const;
	void EnsureViewCreated();
};
