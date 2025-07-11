#include "Rendering/ULUERenderManagerCPU.h"
#include <Ultralight/platform/Config.h>
#include <Ultralight/Ultralight.h>

#include "UltralightUE.h"


ULUERenderManagerCPU::ULUERenderManagerCPU()
{
}

ULUERenderManagerCPU::~ULUERenderManagerCPU()
{
}

ultralight::View* ULUERenderManagerCPU::CreateView(int32 Width, int32 Height, bool bTransparent)
{
	return FUltralightUEModule::Get().GetRenderer()->CreateView(Width, Height, ultralight::ViewConfig(false,false,1.0,bTransparent), nullptr).get();
}

void ULUERenderManagerCPU::RegisterViewTexture(ultralight::View* View, UTexture2D* Texture)
{
	ViewTextureMap.Add(View, Texture);
}

void ULUERenderManagerCPU::UnregisterView(ultralight::View* View)
{
	ViewTextureMap.Remove(View);
}

void ULUERenderManagerCPU::Tick(float DeltaTime)
{
	if (FUltralightUEModule::Get().GetRenderer())
	{
		// Update logic and render all views
		FUltralightUEModule::Get().GetRenderer()->Update();
		FUltralightUEModule::Get().GetRenderer()->Render();

		// Update each registered texture
		for (auto& Pair : ViewTextureMap)
		{
			ultralight::View* View = Pair.Key;
			UTexture2D* Texture = Pair.Value;

			if (View && Texture)
			{
				ultralight::Surface* Surface = View->surface();
				if (ultralight::BitmapSurface* BitmapSurface = static_cast<ultralight::BitmapSurface*>(Surface))
				{
					ultralight::RefPtr<ultralight::Bitmap> Bitmap = BitmapSurface->bitmap();
					void* Pixels = Bitmap->LockPixels();
					uint32_t RowBytes = Bitmap->row_bytes();
					int32_t Width = Bitmap->width();
					int32_t Height = Bitmap->height();

					FUpdateTextureRegion2D Region(0, 0, 0, 0, Width, Height);

					Texture->UpdateTextureRegions(
						0, // Mip level
						1, // Number of regions
						&Region,
						RowBytes,
						4, // Bytes per pixel (BGRA)
						static_cast<uint8_t*>(Pixels),
						[Bitmap](uint8* Data, const FUpdateTextureRegion2D* Region)
						{
							Bitmap->UnlockPixels();
						});
				}
			}
		}
	}
}
