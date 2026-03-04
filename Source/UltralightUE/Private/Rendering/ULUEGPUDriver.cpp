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

#include "Rendering/ULUEGPUDriver.h"
#include "ULUELogging.h"

#include "RenderingThread.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "RHIDefinitions.h"
#include "TextureResource.h"
#include "RenderUtils.h"

#include <Ultralight/Bitmap.h>

namespace ultralightue
{

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

ULUEGPUDriver::ULUEGPUDriver()
{
    UE_LOG(LogUltralightUE, Log, TEXT("ULUEGPUDriver created."));
}

ULUEGPUDriver::~ULUEGPUDriver()
{
    // Ensure all render commands have been flushed before we tear down.
    FlushRenderingCommands();

    Textures.Empty();
    RenderBuffers.Empty();
    Geometries.Empty();

    UE_LOG(LogUltralightUE, Log, TEXT("ULUEGPUDriver destroyed."));
}

// ---------------------------------------------------------------------------
// Synchronization
// ---------------------------------------------------------------------------

void ULUEGPUDriver::BeginSynchronize()
{
    // No-op for now. Could acquire a fence here if needed.
}

void ULUEGPUDriver::EndSynchronize()
{
    // No-op for now. Could signal/wait a fence here if needed.
}

// ---------------------------------------------------------------------------
// Textures
// ---------------------------------------------------------------------------

uint32_t ULUEGPUDriver::NextTextureId()
{
    return NextTexId++;
}

void ULUEGPUDriver::CreateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap)
{
    FULUETextureEntry Entry;
    Entry.Width  = bitmap->width();
    Entry.Height = bitmap->height();
    Entry.bIsRenderTarget = bitmap->IsEmpty();

    // Determine pixel format.
    EPixelFormat Format = PF_B8G8R8A8;
    if (!bitmap->IsEmpty() && bitmap->format() == ultralight::BitmapFormat::A8_UNORM)
    {
        Format = PF_A8;
    }

    // Determine texture creation flags.
    ETextureCreateFlags Flags = ETextureCreateFlags::ShaderResource;
    if (Entry.bIsRenderTarget)
    {
        Flags |= ETextureCreateFlags::RenderTargetable;
    }

    const uint32 TexWidth  = Entry.Width;
    const uint32 TexHeight = Entry.Height;

    // Build the texture descriptor.
    FRHITextureCreateDesc Desc =
        FRHITextureCreateDesc::Create2D(TEXT("UltralightTexture"), TexWidth, TexHeight, Format)
        .SetFlags(Flags)
        .SetNumMips(1);

    // Copy pixel data (if any) before we hand off to the render thread.
    TArray<uint8> PixelData;
    uint32 RowBytes = 0;
    if (!bitmap->IsEmpty())
    {
        RowBytes = bitmap->row_bytes();
        const uint32 DataSize = RowBytes * TexHeight;
        PixelData.SetNumUninitialized(DataSize);

        void* Pixels = bitmap->LockPixels();
        if (Pixels)
        {
            const uint8* SrcRow = static_cast<const uint8*>(Pixels);
            uint8* DstRow = PixelData.GetData();
            for (uint32 Row = 0; Row < TexHeight; ++Row)
            {
                FMemory::Memcpy(DstRow, SrcRow, RowBytes);
                SrcRow += RowBytes;
                DstRow += RowBytes;
            }
        }
        bitmap->UnlockPixels();
    }

    // Create the RHI texture and optionally upload data on the render thread.
    FTexture2DRHIRef* TextureSlot = &Entry.TextureRHI;
    ENQUEUE_RENDER_COMMAND(ULUECreateTexture)(
        [TextureSlot, Desc, PixelData = MoveTemp(PixelData), TexWidth, TexHeight, RowBytes]
        (FRHICommandListImmediate& RHICmdList) mutable
        {
            *TextureSlot = RHICreateTexture(Desc);

            if (PixelData.Num() > 0 && TextureSlot->IsValid())
            {
                FUpdateTextureRegion2D Region(0, 0, 0, 0, TexWidth, TexHeight);
                RHIUpdateTexture2D(*TextureSlot, 0, Region, RowBytes, PixelData.GetData());
            }
        }
    );

    // Block until the render command completes so the texture reference is valid
    // when control returns to Ultralight.
    FlushRenderingCommands();

    Textures.Add(texture_id, MoveTemp(Entry));

    UE_LOG(LogUltralightUE, Verbose,
        TEXT("CreateTexture id=%u  %ux%u  RT=%d"),
        texture_id, TexWidth, TexHeight, Entry.bIsRenderTarget ? 1 : 0);
}

void ULUEGPUDriver::UpdateTexture(uint32_t texture_id, ultralight::RefPtr<ultralight::Bitmap> bitmap)
{
    FULUETextureEntry* Entry = Textures.Find(texture_id);
    if (!Entry || !Entry->TextureRHI.IsValid())
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("UpdateTexture: texture id %u not found."), texture_id);
        return;
    }

    if (bitmap->IsEmpty())
    {
        return; // Nothing to update for render-target-only textures.
    }

    const uint32 TexWidth  = bitmap->width();
    const uint32 TexHeight = bitmap->height();
    const uint32 RowBytes  = bitmap->row_bytes();
    const uint32 DataSize  = RowBytes * TexHeight;

    TArray<uint8> PixelData;
    PixelData.SetNumUninitialized(DataSize);

    void* Pixels = bitmap->LockPixels();
    if (Pixels)
    {
        const uint8* SrcRow = static_cast<const uint8*>(Pixels);
        uint8* DstRow = PixelData.GetData();
        for (uint32 Row = 0; Row < TexHeight; ++Row)
        {
            FMemory::Memcpy(DstRow, SrcRow, RowBytes);
            SrcRow += RowBytes;
            DstRow += RowBytes;
        }
    }
    bitmap->UnlockPixels();

    FTexture2DRHIRef TextureRHI = Entry->TextureRHI;
    ENQUEUE_RENDER_COMMAND(ULUEUpdateTexture)(
        [TextureRHI, PixelData = MoveTemp(PixelData), TexWidth, TexHeight, RowBytes]
        (FRHICommandListImmediate& RHICmdList)
        {
            FUpdateTextureRegion2D Region(0, 0, 0, 0, TexWidth, TexHeight);
            RHIUpdateTexture2D(TextureRHI, 0, Region, RowBytes, PixelData.GetData());
        }
    );

    UE_LOG(LogUltralightUE, Verbose,
        TEXT("UpdateTexture id=%u  %ux%u"), texture_id, TexWidth, TexHeight);
}

void ULUEGPUDriver::DestroyTexture(uint32_t texture_id)
{
    if (Textures.Remove(texture_id) > 0)
    {
        UE_LOG(LogUltralightUE, Verbose, TEXT("DestroyTexture id=%u"), texture_id);
    }
    else
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("DestroyTexture: texture id %u not found."), texture_id);
    }
}

// ---------------------------------------------------------------------------
// Render Buffers
// ---------------------------------------------------------------------------

uint32_t ULUEGPUDriver::NextRenderBufferId()
{
    return NextRBId++;
}

void ULUEGPUDriver::CreateRenderBuffer(uint32_t render_buffer_id, const ultralight::RenderBuffer& buffer)
{
    FULUERenderBufferEntry Entry;
    Entry.TextureId = buffer.texture_id;
    Entry.Width     = buffer.width;
    Entry.Height    = buffer.height;

    // Ensure the associated texture exists and has the correct dimensions.
    // If the texture was originally created at a different size,
    // we may need to recreate it to match the render buffer dimensions.
    FULUETextureEntry* TexEntry = Textures.Find(buffer.texture_id);
    if (TexEntry)
    {
        if (TexEntry->Width != buffer.width || TexEntry->Height != buffer.height)
        {
            UE_LOG(LogUltralightUE, Log,
                TEXT("CreateRenderBuffer: Resizing texture %u from %ux%u to %ux%u"),
                buffer.texture_id, TexEntry->Width, TexEntry->Height,
                buffer.width, buffer.height);

            TexEntry->Width  = buffer.width;
            TexEntry->Height = buffer.height;

            ETextureCreateFlags Flags =
                ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource;

            FRHITextureCreateDesc Desc =
                FRHITextureCreateDesc::Create2D(
                    TEXT("UltralightRBTexture"), buffer.width, buffer.height, PF_B8G8R8A8)
                .SetFlags(Flags)
                .SetNumMips(1);

            FTexture2DRHIRef* TextureSlot = &TexEntry->TextureRHI;
            ENQUEUE_RENDER_COMMAND(ULUEResizeRBTexture)(
                [TextureSlot, Desc](FRHICommandListImmediate& RHICmdList) mutable
                {
                    *TextureSlot = RHICreateTexture(Desc);
                }
            );
            FlushRenderingCommands();
        }
    }
    else
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("CreateRenderBuffer: Texture %u not found for render buffer %u."),
            buffer.texture_id, render_buffer_id);
    }

    RenderBuffers.Add(render_buffer_id, MoveTemp(Entry));

    UE_LOG(LogUltralightUE, Verbose,
        TEXT("CreateRenderBuffer id=%u  tex=%u  %ux%u"),
        render_buffer_id, buffer.texture_id, buffer.width, buffer.height);
}

void ULUEGPUDriver::DestroyRenderBuffer(uint32_t render_buffer_id)
{
    if (RenderBuffers.Remove(render_buffer_id) > 0)
    {
        UE_LOG(LogUltralightUE, Verbose,
            TEXT("DestroyRenderBuffer id=%u"), render_buffer_id);
    }
    else
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("DestroyRenderBuffer: id %u not found."), render_buffer_id);
    }
}

// ---------------------------------------------------------------------------
// Geometry
// ---------------------------------------------------------------------------

uint32_t ULUEGPUDriver::NextGeometryId()
{
    return NextGeoId++;
}

void ULUEGPUDriver::CreateGeometry(uint32_t geometry_id,
                                    const ultralight::VertexBuffer& vertices,
                                    const ultralight::IndexBuffer& indices)
{
    FULUEGeometryEntry Entry;
    Entry.Format = vertices.format;

    // ---- Vertex buffer ----
    const uint32 VBSize = vertices.size;
    {
        FRHIResourceCreateInfo CreateInfo(TEXT("UltralightVB"));
        Entry.VertexBuffer = RHICreateVertexBuffer(VBSize, BUF_Dynamic, CreateInfo);

        void* MappedVB = RHILockBuffer(Entry.VertexBuffer, 0, VBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedVB, vertices.data, VBSize);
        RHIUnlockBuffer(Entry.VertexBuffer);
    }

    // ---- Index buffer ----
    const uint32 IBSize = indices.size;
    {
        FRHIResourceCreateInfo CreateInfo(TEXT("UltralightIB"));
        Entry.IndexBuffer = RHICreateIndexBuffer(sizeof(uint32), IBSize, BUF_Dynamic, CreateInfo);

        void* MappedIB = RHILockBuffer(Entry.IndexBuffer, 0, IBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedIB, indices.data, IBSize);
        RHIUnlockBuffer(Entry.IndexBuffer);
    }

    // Derive counts from sizes assuming IndexType = uint32 (Ultralight standard).
    Entry.IndexCount = IBSize / sizeof(uint32);
    // Vertex count is harder to compute without stride knowledge; store raw size
    // and compute stride from the format when needed.
    Entry.VertexCount = VBSize; // stored as raw byte count; callers derive count from format stride

    Geometries.Add(geometry_id, MoveTemp(Entry));

    UE_LOG(LogUltralightUE, Verbose,
        TEXT("CreateGeometry id=%u  VB=%u bytes  IB=%u bytes"),
        geometry_id, VBSize, IBSize);
}

void ULUEGPUDriver::UpdateGeometry(uint32_t geometry_id,
                                    const ultralight::VertexBuffer& vertices,
                                    const ultralight::IndexBuffer& indices)
{
    FULUEGeometryEntry* Entry = Geometries.Find(geometry_id);
    if (!Entry)
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("UpdateGeometry: geometry id %u not found."), geometry_id);
        return;
    }

    // ---- Vertex buffer ----
    const uint32 VBSize = vertices.size;
    if (Entry->VertexBuffer.IsValid() && Entry->VertexCount == VBSize)
    {
        // Same size -- just lock, copy, unlock.
        void* MappedVB = RHILockBuffer(Entry->VertexBuffer, 0, VBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedVB, vertices.data, VBSize);
        RHIUnlockBuffer(Entry->VertexBuffer);
    }
    else
    {
        // Size changed -- recreate the buffer.
        FRHIResourceCreateInfo CreateInfo(TEXT("UltralightVB"));
        Entry->VertexBuffer = RHICreateVertexBuffer(VBSize, BUF_Dynamic, CreateInfo);

        void* MappedVB = RHILockBuffer(Entry->VertexBuffer, 0, VBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedVB, vertices.data, VBSize);
        RHIUnlockBuffer(Entry->VertexBuffer);
    }
    Entry->VertexCount = VBSize;

    // ---- Index buffer ----
    const uint32 IBSize = indices.size;
    if (Entry->IndexBuffer.IsValid() && (Entry->IndexCount * sizeof(uint32)) == IBSize)
    {
        void* MappedIB = RHILockBuffer(Entry->IndexBuffer, 0, IBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedIB, indices.data, IBSize);
        RHIUnlockBuffer(Entry->IndexBuffer);
    }
    else
    {
        FRHIResourceCreateInfo CreateInfo(TEXT("UltralightIB"));
        Entry->IndexBuffer = RHICreateIndexBuffer(sizeof(uint32), IBSize, BUF_Dynamic, CreateInfo);

        void* MappedIB = RHILockBuffer(Entry->IndexBuffer, 0, IBSize, RLM_WriteOnly);
        FMemory::Memcpy(MappedIB, indices.data, IBSize);
        RHIUnlockBuffer(Entry->IndexBuffer);
    }
    Entry->IndexCount = IBSize / sizeof(uint32);
    Entry->Format = vertices.format;

    UE_LOG(LogUltralightUE, Verbose,
        TEXT("UpdateGeometry id=%u  VB=%u bytes  IB=%u bytes"),
        geometry_id, VBSize, IBSize);
}

void ULUEGPUDriver::DestroyGeometry(uint32_t geometry_id)
{
    if (Geometries.Remove(geometry_id) > 0)
    {
        UE_LOG(LogUltralightUE, Verbose, TEXT("DestroyGeometry id=%u"), geometry_id);
    }
    else
    {
        UE_LOG(LogUltralightUE, Warning,
            TEXT("DestroyGeometry: id %u not found."), geometry_id);
    }
}

// ---------------------------------------------------------------------------
// Command List
// ---------------------------------------------------------------------------

void ULUEGPUDriver::UpdateCommandList(const ultralight::CommandList& list)
{
    FScopeLock Lock(&CommandLock);
    PendingCommands.SetNumUninitialized(list.size);
    if (list.size > 0)
    {
        FMemory::Memcpy(PendingCommands.GetData(), list.commands,
                        list.size * sizeof(ultralight::Command));
    }
}

void ULUEGPUDriver::ExecuteCommands()
{
    // Move commands out under lock so UpdateCommandList can be called concurrently.
    TArray<ultralight::Command> Commands;
    {
        FScopeLock Lock(&CommandLock);
        Commands = MoveTemp(PendingCommands);
        PendingCommands.Reset();
    }

    if (Commands.Num() == 0)
    {
        return;
    }

    // Capture resource maps by pointer (they outlive the render command).
    TMap<uint32, FULUETextureEntry>*       TexMap = &Textures;
    TMap<uint32, FULUERenderBufferEntry>*   RBMap  = &RenderBuffers;
    TMap<uint32, FULUEGeometryEntry>*       GeoMap = &Geometries;

    ENQUEUE_RENDER_COMMAND(ULUEExecuteCommands)(
        [Commands = MoveTemp(Commands), TexMap, RBMap, GeoMap]
        (FRHICommandListImmediate& RHICmdList)
        {
            for (const ultralight::Command& Cmd : Commands)
            {
                if (Cmd.command_type == ultralight::CommandType::ClearRenderBuffer)
                {
                    // ----- Clear render target -----
                    const uint32 RBID = Cmd.gpu_state.render_buffer_id;
                    FULUERenderBufferEntry* RBEntry = RBMap->Find(RBID);
                    if (!RBEntry)
                    {
                        UE_LOG(LogUltralightUE, Warning,
                            TEXT("ExecuteCommands: RenderBuffer %u not found for clear."), RBID);
                        continue;
                    }

                    FULUETextureEntry* TexEntry = TexMap->Find(RBEntry->TextureId);
                    if (!TexEntry || !TexEntry->TextureRHI.IsValid())
                    {
                        UE_LOG(LogUltralightUE, Warning,
                            TEXT("ExecuteCommands: Texture %u not found for clear."),
                            RBEntry->TextureId);
                        continue;
                    }

                    FRHIRenderPassInfo RPInfo(
                        TexEntry->TextureRHI,
                        ERenderTargetActions::Clear_Store);
                    RHICmdList.BeginRenderPass(RPInfo, TEXT("ULClearRT"));
                    RHICmdList.EndRenderPass();
                }
                else if (Cmd.command_type == ultralight::CommandType::DrawGeometry)
                {
                    // ----- Draw geometry (simplified) -----
                    // Full shader-based rendering requires custom global shaders
                    // (see ULUEShaders.h/cpp). For now we set up the render target
                    // and log the draw call so the pipeline is exercised.

                    const uint32 RBID = Cmd.gpu_state.render_buffer_id;
                    FULUERenderBufferEntry* RBEntry = RBMap->Find(RBID);
                    if (!RBEntry)
                    {
                        UE_LOG(LogUltralightUE, Warning,
                            TEXT("ExecuteCommands: RenderBuffer %u not found for draw."),
                            RBID);
                        continue;
                    }

                    FULUETextureEntry* TexEntry = TexMap->Find(RBEntry->TextureId);
                    if (!TexEntry || !TexEntry->TextureRHI.IsValid())
                    {
                        UE_LOG(LogUltralightUE, Warning,
                            TEXT("ExecuteCommands: Texture %u not found for draw."),
                            RBEntry->TextureId);
                        continue;
                    }

                    FULUEGeometryEntry* GeoEntry = GeoMap->Find(Cmd.geometry_id);
                    if (!GeoEntry)
                    {
                        UE_LOG(LogUltralightUE, Warning,
                            TEXT("ExecuteCommands: Geometry %u not found for draw."),
                            Cmd.geometry_id);
                        continue;
                    }

                    // Begin render pass on the target texture.
                    FRHIRenderPassInfo RPInfo(
                        TexEntry->TextureRHI,
                        ERenderTargetActions::Load_Store);
                    RHICmdList.BeginRenderPass(RPInfo, TEXT("ULDrawGeometry"));

                    // TODO: Bind shaders (FUltralightFillVS / FUltralightFillPS or
                    //       FUltralightFillPathVS / FUltralightFillPathPS),
                    //       set viewport, scissor, shader parameters from
                    //       Cmd.gpu_state, then draw indexed.
                    //
                    // Placeholder: The render pass is opened and closed so the
                    // target transitions are correct. Actual draw calls will be
                    // added once the global shaders are fully wired up.

                    RHICmdList.SetViewport(
                        0.0f, 0.0f, 0.0f,
                        static_cast<float>(RBEntry->Width),
                        static_cast<float>(RBEntry->Height),
                        1.0f);

                    if (Cmd.gpu_state.enable_scissor)
                    {
                        RHICmdList.SetScissorRect(
                            true,
                            Cmd.gpu_state.scissor_rect.left,
                            Cmd.gpu_state.scissor_rect.top,
                            Cmd.gpu_state.scissor_rect.right,
                            Cmd.gpu_state.scissor_rect.bottom);
                    }

                    RHICmdList.EndRenderPass();
                }
            }
        }
    );
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FTexture2DRHIRef ULUEGPUDriver::GetRHITexture(uint32_t texture_id) const
{
    const FULUETextureEntry* Entry = Textures.Find(texture_id);
    if (Entry)
    {
        return Entry->TextureRHI;
    }
    return nullptr;
}

} // namespace ultralightue
