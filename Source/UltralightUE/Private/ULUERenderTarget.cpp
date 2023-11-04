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

#include "UltralightUE/Public/ULUERenderTarget.h"
#include "ULUERenderTarget.h"

void ultralightue::ULUERenderTarget::BeginPaint(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
    check(Canvas == nullptr);

    // Setup the viewport
    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
        SetupRenderTargetViewport,
        FTextureRenderTarget2DResource *,
        RenderTarget,
        (FTextureRenderTarget2DResource *)RenderTargetTexture->GameThread_GetRenderTargetResource(),
        {
            SetRenderTarget(RHICmdList, RenderTarget->GetRenderTargetTexture(), FTexture2DRHIRef());
            RHICmdList.SetViewport(0, 0, 0.0f, RenderTarget->GetSizeXY().X, RenderTarget->GetSizeXY().Y, 1.0f);
        });

    Canvas = new (FCanvasBytes) FCanvas(RenderTargetTexture->GameThread_GetRenderTargetResource(), nullptr, InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
}

void ultralightue::ULUERenderTarget::EndPaint()
{
    check(Canvas != nullptr);

    Canvas->Flush_GameThread();
    Canvas->~FCanvas();
    Canvas = nullptr;

    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
        RenderTargetResolve,
        FTextureRenderTargetResource *,
        RenderTargetResource,
        RenderTargetTexture->GameThread_GetRenderTargetResource(),
        {
            RHICmdList.CopyToResolveTarget(RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, true, FResolveParams());
        });
}

void ultralightue::ULUERenderTarget::Destroy()
{
    check(Canvas == nullptr);
    if (RenderTargetTexture)
    {
        RenderTargetTexture->MarkPendingKill();
    }

    MarkPendingKill();
}

ULUERenderTarget *ultralightue::ULUERenderTarget::CreateTransient(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, const FColor &InClearColor)
{
    ULUERenderTarget *target = nullptr;

    if ((InSizeX > 0) && (InSizeY > 0) &&
        ((InSizeX % GPixelFormats[InFormat].BlockSizeX) == 0) &&
        ((InSizeY % GPixelFormats[InFormat].BlockSizeY) == 0))
    {
        Canvas = ConstructObject<URadiantCanvasRenderTarget>(
            ULUERenderTarget::StaticClass(),
            GetTransientPackage(),
            NAME_None,
            RF_Transient);

        Canvas->RenderTargetTexture = ConstructObject<UTextureRenderTarget2D>(
            UTextureRenderTarget2D::StaticClass(),
            GetTransientPackage(),
            NAME_None,
            RF_Transient);

        Canvas->RenderTargetTexture->ClearColor = InClearColor;
        Canvas->RenderTargetTexture->LODGroup = TEXTUREGROUP_UI;
        Canvas->RenderTargetTexture->CompressionSettings = TC_EditorIcon;
        Canvas->RenderTargetTexture->Filter = TF_Default;
        Canvas->RenderTargetTexture->InitCustomFormat(InSizeX, InSizeY, InFormat, true);
        Canvas->RenderTargetTexture->UpdateResourceImmediate();
    }
    else
    {
        UE_LOG(UELog, Warning, TEXT("Invalid parameters specified for ULUERenderTarget::Create(). This may be a fault on Ultralight's Side."));
    }

    return Canvas;
}
