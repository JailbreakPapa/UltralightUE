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

#include "Slate/SUltralightWidget.h"
#include "Slate/ULUEKeyMappings.h"
#include "ULUESubsystem.h"
#include "ULUELogging.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Rendering/DrawElements.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

void SUltralightWidget::Construct(const FArguments& InArgs)
{
	ViewName = InArgs._ViewName;
	Brush = FSlateBrush();
}

void SUltralightWidget::SetViewName(FName InViewName)
{
	ViewName = InViewName;
}

// ---------------------------------------------------------------------------
// Subsystem accessor
// ---------------------------------------------------------------------------

UUltralightSubsystem* SUltralightWidget::GetSubsystem() const
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
// Coordinate mapping
// ---------------------------------------------------------------------------

FVector2D SUltralightWidget::LocalToViewCoords(const FGeometry& MyGeometry, const FVector2D& LocalPos) const
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return LocalPos;
	}

	UTexture2D* Tex = Subsystem->GetViewTexture(ViewName);
	if (!Tex)
	{
		return LocalPos;
	}

	const FVector2D WidgetSize = MyGeometry.GetLocalSize();
	const float ViewW = static_cast<float>(Tex->GetSizeX());
	const float ViewH = static_cast<float>(Tex->GetSizeY());

	if (WidgetSize.X <= 0.0f || WidgetSize.Y <= 0.0f)
	{
		return LocalPos;
	}

	return FVector2D(
		LocalPos.X * (ViewW / WidgetSize.X),
		LocalPos.Y * (ViewH / WidgetSize.Y)
	);
}

int32 SUltralightWidget::MapMouseButton(const FPointerEvent& MouseEvent) const
{
	FKey Button = MouseEvent.GetEffectingButton();
	if (Button == EKeys::LeftMouseButton)   return 0;
	if (Button == EKeys::MiddleMouseButton) return 1;
	if (Button == EKeys::RightMouseButton)  return 2;
	return 0;
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

int32 SUltralightWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
	int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return LayerId;
	}

	UTexture2D* Tex = Subsystem->GetViewTexture(ViewName);
	if (!Tex)
	{
		return LayerId;
	}

	Brush.SetResourceObject(Tex);
	Brush.ImageSize = FVector2D(Tex->GetSizeX(), Tex->GetSizeY());

	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		&Brush,
		DrawEffects,
		InWidgetStyle.GetColorAndOpacityTint()
	);

	return LayerId;
}

FVector2D SUltralightWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		UTexture2D* Tex = Subsystem->GetViewTexture(ViewName);
		if (Tex)
		{
			return FVector2D(
				static_cast<float>(Tex->GetSizeX()),
				static_cast<float>(Tex->GetSizeY())
			);
		}
	}
	return FVector2D(1280.0f, 720.0f);
}

// ---------------------------------------------------------------------------
// Mouse input
// ---------------------------------------------------------------------------

FReply SUltralightWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D ViewPos = LocalToViewCoords(MyGeometry, LocalPos);
	Subsystem->FireMouseMoveEvent(ViewName, ViewPos);
	return FReply::Handled();
}

FReply SUltralightWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D ViewPos = LocalToViewCoords(MyGeometry, LocalPos);
	int32 Button = MapMouseButton(MouseEvent);
	Subsystem->FireMouseButtonEvent(ViewName, ViewPos, Button, true);
	return FReply::Handled().CaptureMouse(SharedThis(this));
}

FReply SUltralightWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D ViewPos = LocalToViewCoords(MyGeometry, LocalPos);
	int32 Button = MapMouseButton(MouseEvent);
	Subsystem->FireMouseButtonEvent(ViewName, ViewPos, Button, false);
	return FReply::Handled().ReleaseMouseCapture();
}

FReply SUltralightWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	float WheelDelta = MouseEvent.GetWheelDelta();
	// Scroll vertically by default; multiply for smooth scrolling
	Subsystem->FireScrollEvent(ViewName, FVector2D(0.0f, WheelDelta * 100.0f));
	return FReply::Handled();
}

// ---------------------------------------------------------------------------
// Keyboard input
// ---------------------------------------------------------------------------

FReply SUltralightWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	int32 KeyCode = ultralightue::MapUEKeyToUltralightKeyCode(InKeyEvent.GetKey());
	if (KeyCode != 0)
	{
		Subsystem->FireKeyEvent(ViewName, KeyCode, true);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SUltralightWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	int32 KeyCode = ultralightue::MapUEKeyToUltralightKeyCode(InKeyEvent.GetKey());
	if (KeyCode != 0)
	{
		Subsystem->FireKeyEvent(ViewName, KeyCode, false);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SUltralightWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (!Subsystem)
	{
		return FReply::Unhandled();
	}

	TCHAR Char = InCharacterEvent.GetCharacter();
	// Skip non-printable control characters except for common ones
	if (Char < 0x20 && Char != TEXT('\t') && Char != TEXT('\r') && Char != TEXT('\n'))
	{
		return FReply::Unhandled();
	}

	FString CharStr;
	CharStr.AppendChar(Char);
	Subsystem->FireCharEvent(ViewName, CharStr);
	return FReply::Handled();
}

// ---------------------------------------------------------------------------
// Focus
// ---------------------------------------------------------------------------

FReply SUltralightWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->SetViewFocus(ViewName, true);
	}
	return FReply::Handled();
}

void SUltralightWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UUltralightSubsystem* Subsystem = GetSubsystem();
	if (Subsystem)
	{
		Subsystem->SetViewFocus(ViewName, false);
	}
}
