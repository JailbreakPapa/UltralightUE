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

#include "ULUEDefines.h"
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ULUERenderManagerCPU.generated.h"

namespace ultralight
{
	class View;
	class Renderer;
}


UCLASS()
class ULTRALIGHTUE_API ULUERenderManagerCPU : public UObject
{
	GENERATED_BODY()
	class Pimpl;

public:
	ULUERenderManagerCPU();
	virtual ~ULUERenderManagerCPU() override;
	// Creates a View with specified dimensions and transparency
	ultralight::View* CreateView(int32 Width, int32 Height, bool bTransparent);

	// Registers a View and its associated texture
	void RegisterViewTexture(ultralight::View* View, UTexture2D* Texture);

	// Unregisters a View when no longer needed
	void UnregisterView(ultralight::View* View);

	// Updates and renders all views, then updates their textures
	void Tick(float DeltaTime);

private:
	Pimpl* m_pimpl;
	TMap<ultralight::View*, UTexture2D*> ViewTextureMap;
};
