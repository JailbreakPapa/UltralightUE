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
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderResource.h"
#include "RHIDefinitions.h"

// ==========================================================================
// Fill shader (quad / full-format vertex)
// ==========================================================================

/** Vertex shader for Ultralight's Fill program (quad geometry, 140-byte vertex). */
class FUltralightFillVS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FUltralightFillVS);
    SHADER_USE_PARAMETER_STRUCT(FUltralightFillVS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(FMatrix44f, Transform)
        SHADER_PARAMETER_ARRAY(FVector4f, Scalar, [8])
        SHADER_PARAMETER_ARRAY(FVector4f, Vector, [8])
        SHADER_PARAMETER(uint32, ClipSize)
        SHADER_PARAMETER_ARRAY(FMatrix44f, Clip, [8])
        SHADER_PARAMETER_TEXTURE(FRHITexture*, Texture1)
        SHADER_PARAMETER_SAMPLER(FRHISamplerState*, Texture1Sampler)
        SHADER_PARAMETER_TEXTURE(FRHITexture*, Texture2)
        SHADER_PARAMETER_SAMPLER(FRHISamplerState*, Texture2Sampler)
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()
};

/** Pixel shader for Ultralight's Fill program. */
class FUltralightFillPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FUltralightFillPS);
    SHADER_USE_PARAMETER_STRUCT(FUltralightFillPS, FGlobalShader);

    using FParameters = FUltralightFillVS::FParameters;
};

// ==========================================================================
// FillPath shader (path / compact-format vertex)
// ==========================================================================

/** Vertex shader for Ultralight's FillPath program (path geometry, 20-byte vertex). */
class FUltralightFillPathVS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FUltralightFillPathVS);
    SHADER_USE_PARAMETER_STRUCT(FUltralightFillPathVS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(FMatrix44f, Transform)
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()
};

/** Pixel shader for Ultralight's FillPath program. */
class FUltralightFillPathPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FUltralightFillPathPS);
    SHADER_USE_PARAMETER_STRUCT(FUltralightFillPathPS, FGlobalShader);

    using FParameters = FUltralightFillPathVS::FParameters;
};

// ==========================================================================
// Vertex declarations (FRenderResource)
// ==========================================================================

/**
 * Vertex declaration for path (compact) geometry.
 *
 * Layout (20 bytes / vertex):
 *   offset  0 : float2   Position   (ATTRIBUTE0, VET_Float2)
 *   offset  8 : uint8x4  Color      (ATTRIBUTE1, VET_Color)
 *   offset 12 : float2   ObjCoord   (ATTRIBUTE2, VET_Float2)
 */
class FUltralightPathVertexDeclaration : public FRenderResource
{
public:
    FVertexDeclarationRHIRef VertexDeclarationRHI;

    virtual void InitRHI() override;
    virtual void ReleaseRHI() override;
};

/**
 * Vertex declaration for quad (full) geometry.
 *
 * Layout (140 bytes / vertex):
 *   offset   0 : float2   Position   (ATTRIBUTE0,  VET_Float2)
 *   offset   8 : uint8x4  Color      (ATTRIBUTE1,  VET_Color)
 *   offset  12 : float2   TexCoord   (ATTRIBUTE2,  VET_Float2)
 *   offset  20 : float2   ObjCoord   (ATTRIBUTE3,  VET_Float2)
 *   offset  28 : float4   Data0      (ATTRIBUTE4,  VET_Float4)
 *   offset  44 : float4   Data1      (ATTRIBUTE5,  VET_Float4)
 *   offset  60 : float4   Data2      (ATTRIBUTE6,  VET_Float4)
 *   offset  76 : float4   Data3      (ATTRIBUTE7,  VET_Float4)
 *   offset  92 : float4   Data4      (ATTRIBUTE8,  VET_Float4)
 *   offset 108 : float4   Data5      (ATTRIBUTE9,  VET_Float4)
 *   offset 124 : float4   Data6      (ATTRIBUTE10, VET_Float4)
 */
class FUltralightQuadVertexDeclaration : public FRenderResource
{
public:
    FVertexDeclarationRHIRef VertexDeclarationRHI;

    virtual void InitRHI() override;
    virtual void ReleaseRHI() override;
};

/** Global instances (created / destroyed with the rendering subsystem). */
extern ULTRALIGHTUE_API TGlobalResource<FUltralightPathVertexDeclaration> GUltralightPathVertexDeclaration;
extern ULTRALIGHTUE_API TGlobalResource<FUltralightQuadVertexDeclaration> GUltralightQuadVertexDeclaration;
