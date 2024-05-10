/*
 *   Copyright (c) 2024 Mikael Aboagye & Ultralight Inc.
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

#include "Rendering/RenderingCommon.h"
#include "UltralightUE.h"
#include "ULUERawMesh.h"

namespace ultralightue
{
    class FULUERenderer
    {
        FULUERenderer();
        FULUERenderer(
            TSharedPtr<FULUERawMesh, ESPMode::ThreadSafe> InBoundMesh,
            const FMatrix &InRenderTransform,
            const FIntRect &InScissorRect)
            : BoundMesh(InBoundMesh), RenderTransform(InRenderTransform), ScissorRect(InScissorRect)
        {
        }

        TSharedPtr<FULUERawMesh, ESPMode::ThreadSafe> BoundMesh;
        FMatrix RenderTransform;
        FIntRect ScissorRect;
    };

    class FULUEGeometryDrawer : public ICustomSlateElement
    {
        FULUEGeometryDrawer(bool bUsing = false);

        virtual void DrawRenderThread(FRHICommandListImmediate &RHICmdList, const void* RenderTarget) override;

        FORCEINLINE void EmplaceMesh(
            TSharedPtr<FULUERawMesh, ESPMode::ThreadSafe> InBoundMesh,
            const FMatrix &InRenderTransform,
            const FIntRect &InScissorRect)
        {
            Renderers.Emplace(InBoundMesh, InRenderTransform, InScissorRect);
        }

        bool IsFree() const { return !bUsing; }

        void MarkUsing() { bUsing = true; }
        void MarkFree() { bUsing = false; }

        private:
        TArray<FULUERawMesh> Renderers;
        bool bUsing = false;
    };

}
