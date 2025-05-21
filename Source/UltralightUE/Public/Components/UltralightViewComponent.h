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
#include "Components/ActorComponent.h"
#include "UltralightViewComponent.generated.h"

class ULUERenderManagerCPU;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULTRALIGHTUE_API UUltralightViewComponent : public UActorComponent
{
	GENERATED_BODY()

	class Pimpl;

public:
	UUltralightViewComponent();
	virtual ~UUltralightViewComponent() override;
	// Properties editable in the Unreal Editor
	UPROPERTY(EditAnywhere, Category = "Ultralight")
	int32 Width = 1920;

	UPROPERTY(EditAnywhere, Category = "Ultralight")
	int32 Height = 1080;

	UPROPERTY(EditAnywhere, Category = "Ultralight")
	bool bTransparent = false;

	UPROPERTY(EditAnywhere, Category = "Ultralight")
	FString URL = TEXT("");

	// The texture containing the rendered Ultralight content
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ultralight")
	UTexture2D* RenderTexture;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

private:
	// Reference to the Ultralight View (pimpl)
	// Internal View: ultralight::RefPtr<ultralight::View> View;
	Pimpl* m_pimpl;
	// Reference to the render manager
	ULUERenderManagerCPU* RenderManager;

	// Helper to initialize the View and texture
	void InitializeUltralightView();
};
