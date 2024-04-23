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

#include "UltralightUE.h"

/// @brief Forward Declaration of ULUE Page view, this is the base component that this class will be rendering.
class ULUEUIPageView;
/// @brief Forward Declaration for ULUE Page Settings, contains settings for transparency, JS Callback functions, etc...
class ULUEUIPageSettings;

namespace ultralightue
{
    UCLASS(meta = (BlueprintSpawnableComponent))
    class ULTRALIGHTUE_EXPORT ULUERenderComponent : public UActorComponent
    {
    public:
        UPROPERTY(EditAnywhere, Category = "UIPageView")
        ULUEUIPageSettings DefaultSettings;

        UPROPERTY(EditAnywhere, Category = "UIPageView")
        ULUEUIPageView UIPageView;


        virtual void Serialize(FArchive &Ar) override;
        virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
        virtual void InitializeComponent() override;
        virtual void OnComponentDestroyed() override;
        
        UFUNCTION(BlueprintCallable, Category = "UIPageView")
        /// @brief Function for updating the render component. this should be called whenever the page view is updated.
        void StartUpdate();

        UFUNCTION(BlueprintCallable, Category = "UIPageView")
        /// @brief Function for stopping the update of the render component. should be used for pages/panels that dont update frequently.
        void StopUpdating();

        UFUNCTION(BlueprintCallable, Category = "UIPageView")
        /// @brief Function for stopping and releasing the render component. this doesn't touch the UIPage at all, and that would need to be released on its own.
        /// For why it doesn't also release the UIPage, a Use-case for this is when we are hiding the UIPage, and dont want to render a blank UICanvas.
        void KillRenderComponent();
    };
}