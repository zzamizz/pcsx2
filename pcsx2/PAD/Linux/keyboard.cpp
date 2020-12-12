/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2020  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

/*
  * Theoretically, this header is for anything to do with keyboard input.
  * Pragmatically, event handing's going in here too.
  */

#if defined(__unix__)
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#elif defined(__APPLE__)
#include <Carbon/Carbon.h>
#endif

#include "keyboard.h"

#if defined(__APPLE__)
std::string KeyName(int pad, int key, int keysym)
{
	// Mouse
	if (keysym >> 16)
	{
		switch (keysym & 0xFFFF)
		{
			case kCGMouseButtonLeft:
				return "Mouse Left";
			case kCGMouseButtonRight:
				return "Mouse Right";
			case kCGMouseButtonCenter:
				return "Mouse Middle";
			default: // Use only number for extra button
				return "Mouse " + std::to_string(keysym & 0xFFFF);
		}
	}

	switch (keysym)
	{
		// clang-format off
		case kVK_ANSI_A:              return "A";
		case kVK_ANSI_B:              return "B";
		case kVK_ANSI_C:              return "C";
		case kVK_ANSI_D:              return "D";
		case kVK_ANSI_E:              return "E";
		case kVK_ANSI_F:              return "F";
		case kVK_ANSI_G:              return "G";
		case kVK_ANSI_H:              return "H";
		case kVK_ANSI_I:              return "I";
		case kVK_ANSI_J:              return "J";
		case kVK_ANSI_K:              return "K";
		case kVK_ANSI_L:              return "L";
		case kVK_ANSI_M:              return "M";
		case kVK_ANSI_N:              return "N";
		case kVK_ANSI_O:              return "O";
		case kVK_ANSI_P:              return "P";
		case kVK_ANSI_Q:              return "Q";
		case kVK_ANSI_R:              return "R";
		case kVK_ANSI_S:              return "S";
		case kVK_ANSI_T:              return "T";
		case kVK_ANSI_U:              return "U";
		case kVK_ANSI_V:              return "V";
		case kVK_ANSI_W:              return "W";
		case kVK_ANSI_X:              return "X";
		case kVK_ANSI_Y:              return "Y";
		case kVK_ANSI_Z:              return "Z";
		case kVK_ANSI_0:              return "0";
		case kVK_ANSI_1:              return "1";
		case kVK_ANSI_2:              return "2";
		case kVK_ANSI_3:              return "3";
		case kVK_ANSI_4:              return "4";
		case kVK_ANSI_5:              return "5";
		case kVK_ANSI_6:              return "6";
		case kVK_ANSI_7:              return "7";
		case kVK_ANSI_8:              return "8";
		case kVK_ANSI_9:              return "9";
		case kVK_ANSI_Grave:          return "`";
		case kVK_ANSI_Minus:          return "-";
		case kVK_ANSI_Equal:          return "=";
		case kVK_ANSI_LeftBracket:    return "[";
		case kVK_ANSI_RightBracket:   return "]";
		case kVK_ANSI_Backslash:      return "\\";
		case kVK_ANSI_Semicolon:      return ";";
		case kVK_ANSI_Quote:          return "'";
		case kVK_ANSI_Comma:          return ",";
		case kVK_ANSI_Period:         return ".";
		case kVK_ANSI_Slash:          return "/";
		case kVK_Escape:              return "⎋";
		case kVK_Tab:                 return "⇥";
		case kVK_Delete:              return "⌫";
		case kVK_ForwardDelete:       return "⌦";
		case kVK_Return:              return "↩";
		case kVK_Space:               return "␣";
		case kVK_ANSI_KeypadDecimal:  return "Keypad .";
		case kVK_ANSI_KeypadMultiply: return "Keypad *";
		case kVK_ANSI_KeypadPlus:     return "Keypad +";
		case kVK_ANSI_KeypadClear:    return "⌧";
		case kVK_ANSI_KeypadDivide:   return "Keypad /";
		case kVK_ANSI_KeypadEnter:    return "⌤";
		case kVK_ANSI_KeypadMinus:    return "Keypad -";
		case kVK_ANSI_KeypadEquals:   return "Keypad =";
		case kVK_ANSI_Keypad0:        return "Keypad 0";
		case kVK_ANSI_Keypad1:        return "Keypad 1";
		case kVK_ANSI_Keypad2:        return "Keypad 2";
		case kVK_ANSI_Keypad3:        return "Keypad 3";
		case kVK_ANSI_Keypad4:        return "Keypad 4";
		case kVK_ANSI_Keypad5:        return "Keypad 5";
		case kVK_ANSI_Keypad6:        return "Keypad 6";
		case kVK_ANSI_Keypad7:        return "Keypad 7";
		case kVK_ANSI_Keypad8:        return "Keypad 8";
		case kVK_ANSI_Keypad9:        return "Keypad 9";
		case kVK_Command:             return "Left ⌘";
		case kVK_Shift:               return "Left ⇧";
		case kVK_CapsLock:            return "⇪";
		case kVK_Option:              return "Left ⌥";
		case kVK_Control:             return "Left ⌃";
		case kVK_RightCommand:        return "Right ⌘";
		case kVK_RightShift:          return "Right ⇧";
		case kVK_RightOption:         return "Right ⌥";
		case kVK_RightControl:        return "Right ⌃";
		case kVK_Function:            return "fn";
		case kVK_VolumeUp:            return "Volume Up";
		case kVK_VolumeDown:          return "Volume Down";
		case kVK_Mute:                return "Mute";
		case kVK_F1:                  return "F1";
		case kVK_F2:                  return "F2";
		case kVK_F3:                  return "F3";
		case kVK_F4:                  return "F4";
		case kVK_F5:                  return "F5";
		case kVK_F6:                  return "F6";
		case kVK_F7:                  return "F7";
		case kVK_F8:                  return "F8";
		case kVK_F9:                  return "F9";
		case kVK_F10:                 return "F10";
		case kVK_F11:                 return "F11";
		case kVK_F12:                 return "F12";
		case kVK_F13:                 return "F13";
		case kVK_F14:                 return "F14";
		case kVK_F15:                 return "F15";
		case kVK_F16:                 return "F16";
		case kVK_F17:                 return "F17";
		case kVK_F18:                 return "F18";
		case kVK_F19:                 return "F19";
		case kVK_F20:                 return "F20";
		case kVK_Help:                return "Help";
		case kVK_Home:                return "↖";
		case kVK_PageUp:              return "⇞";
		case kVK_End:                 return "↘";
		case kVK_PageDown:            return "⇟";
		case kVK_LeftArrow:           return "←";
		case kVK_RightArrow:          return "→";
		case kVK_DownArrow:           return "↓";
		case kVK_UpArrow:             return "↑";
		case kVK_ISO_Section:         return "Section";
		case kVK_JIS_Yen:             return "¥";
		case kVK_JIS_Underscore:      return "_";
		case kVK_JIS_KeypadComma:     return "Keypad ,";
		case kVK_JIS_Eisu:            return "英数";
		case kVK_JIS_Kana:            return "かな";
		default: return "Key " + std::to_string(keysym);
		// clang-format on
	}
}

// Mac keyboard input code is based on Dolphin's Source/Core/InputCommon/ControllerInterface/Quartz/QuartzKeyboardAndMouse.mm
// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+

// All keycodes are 16 bits or less, so we use top 16 bits to differentiate source
// Keyboard keys use discriminator 0
// Mouse buttons use discriminator 1

void UpdateKeyboardInput()
{
	for (int pad = 0; pad < GAMEPAD_NUMBER; pad++)
	{
		const auto& map = g_conf.keysym_map[pad];
		// If we loop over all keys press/release based on current state,
		// joystick axes (which have two bound keys) will always go to the later-polled key
		// Instead, release all keys first and then set the ones that are pressed
		for (const auto& key : map)
			g_key_status.release(pad, key.second);
		for (const auto& key : map)
		{
			bool state;
			if (key.first >> 16 == 0)
			{
				state = CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, key.first);
			}
			else
			{
				state = CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, (CGMouseButton)(key.first & 0xFFFF));
			}
			if (state)
				PressButton(pad, key.second);
		}
	}
}

bool PollForNewKeyboardKeys(u32& pkey)
{
	// All keycodes in <HIToolbox/Events.h> are 0x7e or lower. If you notice
	// keys that aren't being recognized, bump this number up!
	for (int key = 0; key < 0x80; key++)
	{
		if (CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, key))
		{
			pkey = key == kVK_Escape ? UINT32_MAX : key;
			return true;
		}
	}
	for (auto btn : {kCGMouseButtonLeft, kCGMouseButtonCenter, kCGMouseButtonRight})
	{
		if (CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, btn))
		{
			pkey = btn | (1 << 16);
			return true;
		}
	}
	return false;
}
#elif defined(__unix__)

std::string KeyName(int pad, int key, int keysym)
{
	// Mouse
	if (keysym < 10)
	{
		switch (keysym)
		{
			case 0:
				return "";
			case 1:
				return "Mouse Left";
			case 2:
				return "Mouse Middle";
			case 3:
				return "Mouse Right";
			default: // Use only number for extra button
				return "Mouse " + std::to_string(keysym);
		}
	}

	return std::string(XKeysymToString(keysym));
}

static bool s_grab_input = false;
static bool s_Shift = false;
static unsigned int s_previous_mouse_x = 0;
static unsigned int s_previous_mouse_y = 0;

void AnalyzeKeyEvent(keyEvent& evt)
{
	KeySym key = (KeySym)evt.key;
	int pad = 0;
	int index = -1;

	for (int cpad = 0; cpad < GAMEPAD_NUMBER; cpad++)
	{
		int tmp_index = get_keyboard_key(cpad, key);
		if (tmp_index != -1)
		{
			pad = cpad;
			index = tmp_index;
		}
	}

	switch (evt.evt)
	{
		case KeyPress:
			// Shift F12 is not yet use by pcsx2. So keep it to grab/ungrab input
			// I found it very handy vs the automatic fullscreen detection
			// 1/ Does not need to detect full-screen
			// 2/ Can use a debugger in full-screen
			// 3/ Can grab input in window without the need of a pixelated full-screen
			if (key == XK_Shift_R || key == XK_Shift_L)
				s_Shift = true;
			if (key == XK_F12 && s_Shift)
			{
				if (!s_grab_input)
				{
					s_grab_input = true;
					XGrabPointer(GSdsp, GSwin, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, GSwin, None, CurrentTime);
					XGrabKeyboard(GSdsp, GSwin, True, GrabModeAsync, GrabModeAsync, CurrentTime);
				}
				else
				{
					s_grab_input = false;
					XUngrabPointer(GSdsp, CurrentTime);
					XUngrabKeyboard(GSdsp, CurrentTime);
				}
			}

			if (index != -1)
				PressButton(pad, index);

			//PAD_LOG("Key pressed:%d\n", index);

			event.evt = KEYPRESS;
			event.key = key;
			break;

		case KeyRelease:
			if (key == XK_Shift_R || key == XK_Shift_L)
				s_Shift = false;

			if (index != -1)
				g_key_status.release(pad, index);

			event.evt = KEYRELEASE;
			event.key = key;
			break;

		case FocusIn:
			break;

		case FocusOut:
			s_Shift = false;
			break;

		case ButtonPress:
			if (index != -1)
				g_key_status.press(pad, index);
			break;

		case ButtonRelease:
			if (index != -1)
				g_key_status.release(pad, index);
			break;

		case MotionNotify:
			// FIXME: How to handle when the mouse does not move, no event generated!!!
			// 1/ small move == no move. Cons : can not do small movement
			// 2/ use a watchdog timer thread
			// 3/ ??? idea welcome ;)
			if (g_conf.pad_options[pad].mouse_l | g_conf.pad_options[pad].mouse_r)
			{
				unsigned int pad_x;
				unsigned int pad_y;
				// Note when both PADOPTION_MOUSE_R and PADOPTION_MOUSE_L are set, take only the right one
				if (g_conf.pad_options[pad].mouse_r)
				{
					pad_x = PAD_R_RIGHT;
					pad_y = PAD_R_UP;
				}
				else
				{
					pad_x = PAD_L_RIGHT;
					pad_y = PAD_L_UP;
				}

				unsigned x = evt.key & 0xFFFF;
				unsigned int value = (s_previous_mouse_x > x) ? s_previous_mouse_x - x : x - s_previous_mouse_x;
				value *= g_conf.get_sensibility();

				if (x == 0)
					g_key_status.press(pad, pad_x, -MAX_ANALOG_VALUE);
				else if (x == 0xFFFF)
					g_key_status.press(pad, pad_x, MAX_ANALOG_VALUE);
				else if (x < (s_previous_mouse_x - 2))
					g_key_status.press(pad, pad_x, -value);
				else if (x > (s_previous_mouse_x + 2))
					g_key_status.press(pad, pad_x, value);
				else
					g_key_status.release(pad, pad_x);


				unsigned y = evt.key >> 16;
				value = (s_previous_mouse_y > y) ? s_previous_mouse_y - y : y - s_previous_mouse_y;
				value *= g_conf.get_sensibility();

				if (y == 0)
					g_key_status.press(pad, pad_y, -MAX_ANALOG_VALUE);
				else if (y == 0xFFFF)
					g_key_status.press(pad, pad_y, MAX_ANALOG_VALUE);
				else if (y < (s_previous_mouse_y - 2))
					g_key_status.press(pad, pad_y, -value);
				else if (y > (s_previous_mouse_y + 2))
					g_key_status.press(pad, pad_y, value);
				else
					g_key_status.release(pad, pad_y);

				s_previous_mouse_x = x;
				s_previous_mouse_y = y;
			}

			break;
	}
}

void UpdateKeyboardInput()
{
	keyEvent evt = {0};
	XEvent E = {0};

	// Keyboard input send by PCSX2
	g_ev_fifo.consume_all(AnalyzeKeyEvent);

	// keyboard input
	while (XPending(GSdsp) > 0)
	{
		XNextEvent(GSdsp, &E);

		// Change the format of the structure to be compatible with GSOpen2
		// mode (event come from pcsx2 not X)
		evt.evt = E.type;
		switch (E.type)
		{
			case MotionNotify:
				evt.key = (E.xbutton.x & 0xFFFF) | (E.xbutton.y << 16);
				break;
			case ButtonRelease:
			case ButtonPress:
				evt.key = E.xbutton.button;
				break;
			default:
				evt.key = (int)XLookupKeysym(&E.xkey, 0);
		}

		AnalyzeKeyEvent(evt);
	}
}

bool PollForNewKeyboardKeys(u32& pkey)
{
	GdkEvent* ev = gdk_event_get();

	if (ev != NULL)
	{
		if (ev->type == GDK_KEY_PRESS)
		{
			pkey = ev->key.keyval != GDK_KEY_Escape ? ev->key.keyval : UINT32_MAX;
			return true;
		}
		else if (ev->type == GDK_BUTTON_PRESS)
		{
			pkey = ev->button.button;
			return true;
		}
	}

	return false;
}

#endif
