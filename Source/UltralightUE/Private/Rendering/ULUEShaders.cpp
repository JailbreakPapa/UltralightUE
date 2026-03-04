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

#include "Rendering/ULUEShaders.h"
#include "ULUELogging.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

// ==========================================================================
// Global shader implementations
// ==========================================================================

// Fill (quad) shaders -- /Plugin/UltralightUE/Private/UltralightFill.usf

IMPLEMENT_GLOBAL_SHADER(
    FUltralightFillVS,
    "/Plugin/UltralightUE/Private/UltralightFill",
    "MainVS",
    SF_Vertex);

IMPLEMENT_GLOBAL_SHADER(
    FUltralightFillPS,
    "/Plugin/UltralightUE/Private/UltralightFill",
    "MainPS",
    SF_Pixel);

// FillPath (path) shaders -- /Plugin/UltralightUE/Private/UltralightFillPath.usf

IMPLEMENT_GLOBAL_SHADER(
    FUltralightFillPathVS,
    "/Plugin/UltralightUE/Private/UltralightFillPath",
    "MainVS",
    SF_Vertex);

IMPLEMENT_GLOBAL_SHADER(
    FUltralightFillPathPS,
    "/Plugin/UltralightUE/Private/UltralightFillPath",
    "MainPS",
    SF_Pixel);

// ==========================================================================
// Vertex declarations
// ==========================================================================

TGlobalResource<FUltralightPathVertexDeclaration> GUltralightPathVertexDeclaration;
TGlobalResource<FUltralightQuadVertexDeclaration> GUltralightQuadVertexDeclaration;

// --------------------------------------------------------------------------
// Path vertex declaration  (20 bytes / vertex)
// --------------------------------------------------------------------------

void FUltralightPathVertexDeclaration::InitRHI()
{
    static constexpr uint16 Stride = 20; // sizeof path vertex

    FVertexDeclarationElementList Elements;
    // float2 Position  @ offset 0  -> ATTRIBUTE0
    Elements.Add(FVertexElement(0, 0,  VET_Float2, 0, Stride));
    // uint8x4 Color    @ offset 8  -> ATTRIBUTE1
    Elements.Add(FVertexElement(0, 8,  VET_Color,  1, Stride));
    // float2 ObjCoord  @ offset 12 -> ATTRIBUTE2
    Elements.Add(FVertexElement(0, 12, VET_Float2, 2, Stride));

    VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);

    UE_LOG(LogUltralightUE, Log,
        TEXT("FUltralightPathVertexDeclaration initialised (stride=%u)."), Stride);
}

void FUltralightPathVertexDeclaration::ReleaseRHI()
{
    VertexDeclarationRHI.SafeRelease();
}

// --------------------------------------------------------------------------
// Quad vertex declaration  (140 bytes / vertex)
// --------------------------------------------------------------------------

void FUltralightQuadVertexDeclaration::InitRHI()
{
    static constexpr uint16 Stride = 140; // sizeof quad vertex

    FVertexDeclarationElementList Elements;
    // float2 Position  @ offset   0 -> ATTRIBUTE0
    Elements.Add(FVertexElement(0,   0, VET_Float2, 0,  Stride));
    // uint8x4 Color    @ offset   8 -> ATTRIBUTE1
    Elements.Add(FVertexElement(0,   8, VET_Color,  1,  Stride));
    // float2 TexCoord  @ offset  12 -> ATTRIBUTE2
    Elements.Add(FVertexElement(0,  12, VET_Float2, 2,  Stride));
    // float2 ObjCoord  @ offset  20 -> ATTRIBUTE3
    Elements.Add(FVertexElement(0,  20, VET_Float2, 3,  Stride));
    // float4 Data0     @ offset  28 -> ATTRIBUTE4
    Elements.Add(FVertexElement(0,  28, VET_Float4, 4,  Stride));
    // float4 Data1     @ offset  44 -> ATTRIBUTE5
    Elements.Add(FVertexElement(0,  44, VET_Float4, 5,  Stride));
    // float4 Data2     @ offset  60 -> ATTRIBUTE6
    Elements.Add(FVertexElement(0,  60, VET_Float4, 6,  Stride));
    // float4 Data3     @ offset  76 -> ATTRIBUTE7
    Elements.Add(FVertexElement(0,  76, VET_Float4, 7,  Stride));
    // float4 Data4     @ offset  92 -> ATTRIBUTE8
    Elements.Add(FVertexElement(0,  92, VET_Float4, 8,  Stride));
    // float4 Data5     @ offset 108 -> ATTRIBUTE9
    Elements.Add(FVertexElement(0, 108, VET_Float4, 9,  Stride));
    // float4 Data6     @ offset 124 -> ATTRIBUTE10
    Elements.Add(FVertexElement(0, 124, VET_Float4, 10, Stride));

    VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);

    UE_LOG(LogUltralightUE, Log,
        TEXT("FUltralightQuadVertexDeclaration initialised (stride=%u)."), Stride);
}

void FUltralightQuadVertexDeclaration::ReleaseRHI()
{
    VertexDeclarationRHI.SafeRelease();
}
