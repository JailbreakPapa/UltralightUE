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

#include "Widget/ULUEWidget.h"
#include "Slate/SUltralightWidget.h"
#include "ULUESubsystem.h"
#include "ULUELogging.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

#define LOCTEXT_NAMESPACE "UltralightWidget"

// ---------------------------------------------------------------------------
// Subsystem accessor
// ---------------------------------------------------------------------------

UUltralightSubsystem* UUltralightWidget::GetSubsystem() const
{
	if (!GEngine)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		return nullptr;
	}

	return GI->GetSubsystem<UUltralightSubsystem>();
}

// ---------------------------------------------------------------------------
// UWidget interface
// ---------------------------------------------------------------------------

TSharedRef<SWidget> UUltralightWidget::RebuildWidget()
{
	SlateWidget = SNew(SUltralightWidget)
		.ViewName(ViewName);

	EnsureViewCreated();

	return SlateWidget.ToSharedRef();
}

void UUltralightWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem && bViewCreated)
	{
		Subsystem->DestroyView(ViewName);
	}

	bViewCreated = false;
	SlateWidget.Reset();
}

#if WITH_EDITOR
const FText UUltralightWidget::GetPaletteCategory()
{
	return LOCTEXT("UltralightUE", "Ultralight");
}
#endif

// ---------------------------------------------------------------------------
// View lifecycle
// ---------------------------------------------------------------------------

void UUltralightWidget::EnsureViewCreated()
{
	if (bViewCreated)
	{
		return;
	}

	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return;
	}

	FName CreatedName = Subsystem->CreateView(ViewName, ViewConfig);
	if (CreatedName != NAME_None)
	{
		bViewCreated = true;

		if (!InitialURL.IsEmpty())
		{
			Subsystem->LoadURL(ViewName, InitialURL);
		}

		UE_LOG(LogUltralightUE, Log, TEXT("UUltralightWidget: Created view '%s'."), *ViewName.ToString());
	}
}

// ---------------------------------------------------------------------------
// Blueprint API
// ---------------------------------------------------------------------------

void UUltralightWidget::LoadURL(const FString& URL)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->LoadURL(ViewName, URL);
	}
}

void UUltralightWidget::LoadHTML(const FString& HTMLContent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->LoadHTML(ViewName, HTMLContent);
	}
}

FString UUltralightWidget::ExecuteJavaScript(const FString& Script)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		return Subsystem->EvaluateJavaScript(ViewName, Script);
	}
	return FString();
}

void UUltralightWidget::Reload()
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->Reload(ViewName);
	}
}

void UUltralightWidget::GoBack()
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->GoBack(ViewName);
	}
}

void UUltralightWidget::GoForward()
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->GoForward(ViewName);
	}
}

#undef LOCTEXT_NAMESPACE
