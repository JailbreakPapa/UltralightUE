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

#include "Slate/ULUEKeyMappings.h"
#include <Ultralight/KeyCodes.h>
#include <Ultralight/KeyEvent.h>

namespace ultralightue
{

int32 MapUEKeyToUltralightKeyCode(const FKey& Key)
{
	static TMap<FKey, int32> KeyMap = []()
	{
		TMap<FKey, int32> Map;

		// Letters A-Z
		Map.Add(EKeys::A, ultralight::KeyCodes::GK_A);
		Map.Add(EKeys::B, ultralight::KeyCodes::GK_B);
		Map.Add(EKeys::C, ultralight::KeyCodes::GK_C);
		Map.Add(EKeys::D, ultralight::KeyCodes::GK_D);
		Map.Add(EKeys::E, ultralight::KeyCodes::GK_E);
		Map.Add(EKeys::F, ultralight::KeyCodes::GK_F);
		Map.Add(EKeys::G, ultralight::KeyCodes::GK_G);
		Map.Add(EKeys::H, ultralight::KeyCodes::GK_H);
		Map.Add(EKeys::I, ultralight::KeyCodes::GK_I);
		Map.Add(EKeys::J, ultralight::KeyCodes::GK_J);
		Map.Add(EKeys::K, ultralight::KeyCodes::GK_K);
		Map.Add(EKeys::L, ultralight::KeyCodes::GK_L);
		Map.Add(EKeys::M, ultralight::KeyCodes::GK_M);
		Map.Add(EKeys::N, ultralight::KeyCodes::GK_N);
		Map.Add(EKeys::O, ultralight::KeyCodes::GK_O);
		Map.Add(EKeys::P, ultralight::KeyCodes::GK_P);
		Map.Add(EKeys::Q, ultralight::KeyCodes::GK_Q);
		Map.Add(EKeys::R, ultralight::KeyCodes::GK_R);
		Map.Add(EKeys::S, ultralight::KeyCodes::GK_S);
		Map.Add(EKeys::T, ultralight::KeyCodes::GK_T);
		Map.Add(EKeys::U, ultralight::KeyCodes::GK_U);
		Map.Add(EKeys::V, ultralight::KeyCodes::GK_V);
		Map.Add(EKeys::W, ultralight::KeyCodes::GK_W);
		Map.Add(EKeys::X, ultralight::KeyCodes::GK_X);
		Map.Add(EKeys::Y, ultralight::KeyCodes::GK_Y);
		Map.Add(EKeys::Z, ultralight::KeyCodes::GK_Z);

		// Numbers 0-9
		Map.Add(EKeys::Zero, ultralight::KeyCodes::GK_0);
		Map.Add(EKeys::One, ultralight::KeyCodes::GK_1);
		Map.Add(EKeys::Two, ultralight::KeyCodes::GK_2);
		Map.Add(EKeys::Three, ultralight::KeyCodes::GK_3);
		Map.Add(EKeys::Four, ultralight::KeyCodes::GK_4);
		Map.Add(EKeys::Five, ultralight::KeyCodes::GK_5);
		Map.Add(EKeys::Six, ultralight::KeyCodes::GK_6);
		Map.Add(EKeys::Seven, ultralight::KeyCodes::GK_7);
		Map.Add(EKeys::Eight, ultralight::KeyCodes::GK_8);
		Map.Add(EKeys::Nine, ultralight::KeyCodes::GK_9);

		// Function keys
		Map.Add(EKeys::F1, ultralight::KeyCodes::GK_F1);
		Map.Add(EKeys::F2, ultralight::KeyCodes::GK_F2);
		Map.Add(EKeys::F3, ultralight::KeyCodes::GK_F3);
		Map.Add(EKeys::F4, ultralight::KeyCodes::GK_F4);
		Map.Add(EKeys::F5, ultralight::KeyCodes::GK_F5);
		Map.Add(EKeys::F6, ultralight::KeyCodes::GK_F6);
		Map.Add(EKeys::F7, ultralight::KeyCodes::GK_F7);
		Map.Add(EKeys::F8, ultralight::KeyCodes::GK_F8);
		Map.Add(EKeys::F9, ultralight::KeyCodes::GK_F9);
		Map.Add(EKeys::F10, ultralight::KeyCodes::GK_F10);
		Map.Add(EKeys::F11, ultralight::KeyCodes::GK_F11);
		Map.Add(EKeys::F12, ultralight::KeyCodes::GK_F12);

		// Navigation / editing
		Map.Add(EKeys::SpaceBar, ultralight::KeyCodes::GK_SPACE);
		Map.Add(EKeys::Enter, ultralight::KeyCodes::GK_RETURN);
		Map.Add(EKeys::Tab, ultralight::KeyCodes::GK_TAB);
		Map.Add(EKeys::BackSpace, ultralight::KeyCodes::GK_BACK);
		Map.Add(EKeys::Escape, ultralight::KeyCodes::GK_ESCAPE);
		Map.Add(EKeys::Delete, ultralight::KeyCodes::GK_DELETE);
		Map.Add(EKeys::Insert, ultralight::KeyCodes::GK_INSERT);

		// Arrow keys
		Map.Add(EKeys::Left, ultralight::KeyCodes::GK_LEFT);
		Map.Add(EKeys::Right, ultralight::KeyCodes::GK_RIGHT);
		Map.Add(EKeys::Up, ultralight::KeyCodes::GK_UP);
		Map.Add(EKeys::Down, ultralight::KeyCodes::GK_DOWN);

		// Home / End / Page
		Map.Add(EKeys::Home, ultralight::KeyCodes::GK_HOME);
		Map.Add(EKeys::End, ultralight::KeyCodes::GK_END);
		Map.Add(EKeys::PageUp, ultralight::KeyCodes::GK_PRIOR);
		Map.Add(EKeys::PageDown, ultralight::KeyCodes::GK_NEXT);

		// Modifier keys
		Map.Add(EKeys::LeftShift, ultralight::KeyCodes::GK_LSHIFT);
		Map.Add(EKeys::RightShift, ultralight::KeyCodes::GK_RSHIFT);
		Map.Add(EKeys::LeftControl, ultralight::KeyCodes::GK_LCONTROL);
		Map.Add(EKeys::RightControl, ultralight::KeyCodes::GK_RCONTROL);
		Map.Add(EKeys::LeftAlt, ultralight::KeyCodes::GK_LMENU);
		Map.Add(EKeys::RightAlt, ultralight::KeyCodes::GK_RMENU);
		Map.Add(EKeys::LeftCommand, ultralight::KeyCodes::GK_LWIN);
		Map.Add(EKeys::RightCommand, ultralight::KeyCodes::GK_RWIN);

		// Lock keys
		Map.Add(EKeys::CapsLock, ultralight::KeyCodes::GK_CAPITAL);
		Map.Add(EKeys::NumLock, ultralight::KeyCodes::GK_NUMLOCK);
		Map.Add(EKeys::ScrollLock, ultralight::KeyCodes::GK_SCROLL);

		// Numpad
		Map.Add(EKeys::NumPadZero, ultralight::KeyCodes::GK_NUMPAD0);
		Map.Add(EKeys::NumPadOne, ultralight::KeyCodes::GK_NUMPAD1);
		Map.Add(EKeys::NumPadTwo, ultralight::KeyCodes::GK_NUMPAD2);
		Map.Add(EKeys::NumPadThree, ultralight::KeyCodes::GK_NUMPAD3);
		Map.Add(EKeys::NumPadFour, ultralight::KeyCodes::GK_NUMPAD4);
		Map.Add(EKeys::NumPadFive, ultralight::KeyCodes::GK_NUMPAD5);
		Map.Add(EKeys::NumPadSix, ultralight::KeyCodes::GK_NUMPAD6);
		Map.Add(EKeys::NumPadSeven, ultralight::KeyCodes::GK_NUMPAD7);
		Map.Add(EKeys::NumPadEight, ultralight::KeyCodes::GK_NUMPAD8);
		Map.Add(EKeys::NumPadNine, ultralight::KeyCodes::GK_NUMPAD9);
		Map.Add(EKeys::Multiply, ultralight::KeyCodes::GK_MULTIPLY);
		Map.Add(EKeys::Add, ultralight::KeyCodes::GK_ADD);
		Map.Add(EKeys::Subtract, ultralight::KeyCodes::GK_SUBTRACT);
		Map.Add(EKeys::Decimal, ultralight::KeyCodes::GK_DECIMAL);
		Map.Add(EKeys::Divide, ultralight::KeyCodes::GK_DIVIDE);

		// Punctuation / symbols
		Map.Add(EKeys::Semicolon, ultralight::KeyCodes::GK_OEM_1);
		Map.Add(EKeys::Equals, ultralight::KeyCodes::GK_OEM_PLUS);
		Map.Add(EKeys::Comma, ultralight::KeyCodes::GK_OEM_COMMA);
		Map.Add(EKeys::Hyphen, ultralight::KeyCodes::GK_OEM_MINUS);
		Map.Add(EKeys::Period, ultralight::KeyCodes::GK_OEM_PERIOD);
		Map.Add(EKeys::Slash, ultralight::KeyCodes::GK_OEM_2);
		Map.Add(EKeys::Tilde, ultralight::KeyCodes::GK_OEM_3);
		Map.Add(EKeys::LeftBracket, ultralight::KeyCodes::GK_OEM_4);
		Map.Add(EKeys::Backslash, ultralight::KeyCodes::GK_OEM_5);
		Map.Add(EKeys::RightBracket, ultralight::KeyCodes::GK_OEM_6);
		Map.Add(EKeys::Quote, ultralight::KeyCodes::GK_OEM_7);

		// Misc
		Map.Add(EKeys::Pause, ultralight::KeyCodes::GK_PAUSE);

		return Map;
	}();

	const int32* Found = KeyMap.Find(Key);
	return Found ? *Found : ultralight::KeyCodes::GK_UNKNOWN;
}

unsigned MapUEModifiers(bool bAlt, bool bCtrl, bool bShift, bool bCmd)
{
	unsigned Modifiers = 0;
	if (bAlt)   Modifiers |= ultralight::KeyEvent::kMod_AltKey;
	if (bCtrl)  Modifiers |= ultralight::KeyEvent::kMod_CtrlKey;
	if (bShift) Modifiers |= ultralight::KeyEvent::kMod_ShiftKey;
	if (bCmd)   Modifiers |= ultralight::KeyEvent::kMod_MetaKey;
	return Modifiers;
}

} // namespace ultralightue
