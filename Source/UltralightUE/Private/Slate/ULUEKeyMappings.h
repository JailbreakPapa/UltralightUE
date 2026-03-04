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
#include "InputCoreTypes.h"

namespace ultralightue
{
	/**
	 * Maps an Unreal Engine FKey to the corresponding Ultralight virtual key code.
	 *
	 * @param Key  The Unreal Engine key to map.
	 * @return     The Ultralight virtual key code, or 0 if no mapping exists.
	 */
	int32 MapUEKeyToUltralightKeyCode(const FKey& Key);

	/**
	 * Converts Unreal Engine modifier key states to Ultralight modifier flags.
	 *
	 * @param bAlt    Whether the Alt key is pressed.
	 * @param bCtrl   Whether the Ctrl key is pressed.
	 * @param bShift  Whether the Shift key is pressed.
	 * @param bCmd    Whether the Command/Meta key is pressed.
	 * @return        A bitmask of ultralight::KeyEvent::Modifiers values.
	 */
	unsigned MapUEModifiers(bool bAlt, bool bCtrl, bool bShift, bool bCmd);
}
