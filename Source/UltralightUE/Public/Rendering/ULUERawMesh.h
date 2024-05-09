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

class FULUETextureHandle;
struct FULUEVertData;

namespace ultralightue
{
    class FULUERawMesh : public TSharedFromThis<FULUERawMesh, ESPMode::ThreadSafe>
    {
    public:
        struct FVertexData
        {
            FVector2D Position;
            FColor Color;
            FVector2D UV;
            FVertexData(const FVector2D &InPos, const FVector2D &InUV, const FColor &InColor)
                : Position(InPos), Color(InColor), UV(InUV)
            {
            }
        };
        void Setup(FLULUEVertData *vertices, int num_vertices, int *indices, int num_indices, TSharedPtr<FULUETextureHandle, ESPMode::ThreadSafe> InTexture);
        void BuildMesh();
        void ReleaseMesh();
        void DrawMesh(FRHICommandList &RHICmdList);

    public:
        TResourceArray<FVertexData> Vertices;
        FVertexBufferRHIRef VertexBufferRHI;

        TResourceArray<uint16> Indices;
        FIndexBufferRHIRef IndexBufferRHI;

        int32 NumVertices;
        int32 NumTriangles;

        TSharedPtr<FULUETextureHandle, ESPMode::ThreadSafe> BoundTexture;
    };
}