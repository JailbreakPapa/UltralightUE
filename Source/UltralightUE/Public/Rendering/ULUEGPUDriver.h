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
#include "RHI.h"
#include "RHIResources.h"
#include <Ultralight/platform/GPUDriver.h>

namespace ultralightue
{
    /** Holds the RHI texture reference and metadata for a single Ultralight texture. */
    struct FULUETextureEntry
    {
        FTexture2DRHIRef TextureRHI;
        uint32 Width = 0;
        uint32 Height = 0;
        bool bIsRenderTarget = false;
    };

    /** Maps a render buffer id to its associated texture id and dimensions. */
    struct FULUERenderBufferEntry
    {
        uint32 TextureId = 0;
        uint32 Width = 0;
        uint32 Height = 0;
    };

    /** Holds vertex and index buffer RHI references for a single geometry entry. */
    struct FULUEGeometryEntry
    {
        FBufferRHIRef VertexBuffer;
        FBufferRHIRef IndexBuffer;
        ultralight::VertexBufferFormat Format;
        uint32 VertexCount = 0;
        uint32 IndexCount = 0;
    };

    /**
     * Implements ultralight::GPUDriver using Unreal Engine's RHI.
     *
     * This driver translates Ultralight GPU rendering commands into UE RHI
     * operations, allowing Ultralight to render web content using the same
     * graphics backend as the engine.
     */
    class ULTRALIGHTUE_API ULUEGPUDriver : public ultralight::GPUDriver
    {
    public:
        ULUEGPUDriver();
        virtual ~ULUEGPUDriver() override;

        // ---- ultralight::GPUDriver interface ----

        virtual void BeginSynchronize() override;
        virtual void EndSynchronize() override;

        virtual uint32_t NextTextureId() override;
        virtual void CreateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap) override;
        virtual void UpdateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap) override;
        virtual void DestroyTexture(uint32_t texture_id) override;

        virtual uint32_t NextRenderBufferId() override;
        virtual void CreateRenderBuffer(uint32_t render_buffer_id, const ultralight::RenderBuffer& buffer) override;
        virtual void DestroyRenderBuffer(uint32_t render_buffer_id) override;

        virtual uint32_t NextGeometryId() override;
        virtual void CreateGeometry(uint32_t geometry_id, const ultralight::VertexBuffer& vertices, const ultralight::IndexBuffer& indices) override;
        virtual void UpdateGeometry(uint32_t geometry_id, const ultralight::VertexBuffer& vertices, const ultralight::IndexBuffer& indices) override;
        virtual void DestroyGeometry(uint32_t geometry_id) override;

        virtual void UpdateCommandList(const ultralight::CommandList& list) override;

        // ---- Public helpers ----

        /** Execute all pending GPU commands. Called by the subsystem after Renderer::Render(). */
        void ExecuteCommands();

        /** Retrieve the RHI texture associated with the given Ultralight texture id. */
        FTexture2DRHIRef GetRHITexture(uint32_t texture_id) const;

    private:
        uint32 NextTexId = 1;
        uint32 NextRBId = 1;
        uint32 NextGeoId = 1;

        TMap<uint32, FULUETextureEntry> Textures;
        TMap<uint32, FULUERenderBufferEntry> RenderBuffers;
        TMap<uint32, FULUEGeometryEntry> Geometries;

        /** Copied command list, guarded by CommandLock. */
        TArray<ultralight::Command> PendingCommands;
        FCriticalSection CommandLock;
    };
}
