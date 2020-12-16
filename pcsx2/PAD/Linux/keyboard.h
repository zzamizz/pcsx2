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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "PAD.h"
#include "config.h"

#if defined(__unix__) || defined(__APPLE__)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
// x11 is dumb like that
#undef DisableScreenSaver

extern void AnalyzeKeyEvent(keyEvent& evt);
extern void UpdateKeyboardInput();
extern bool PollForNewKeyboardKeys(u32& pkey);
extern std::string KeyName(int pad, int key, int keysym);

[[maybe_unused]]static std::map<int, int> keyboard_defaults =
{
	{PAD_L2, XK_a},
	{PAD_R2, XK_semicolon},
	{PAD_L1, XK_w},
	{PAD_R1, XK_p},

	{PAD_TRIANGLE, XK_i},
	{PAD_CIRCLE, XK_l},
	{PAD_CROSS, XK_k},
	{PAD_SQUARE, XK_j},

	{PAD_SELECT, XK_v},
	//{PAD_L3, XK_KP_Enter},
	//{PAD_R3, XK_KP_Add},
	{PAD_START, XK_n},

	{PAD_UP, XK_e},
	{PAD_RIGHT, XK_f},
	{PAD_DOWN, XK_d},
	{PAD_LEFT, XK_s},

	//{PAD_L_UP, XK_Up},
	//{PAD_L_RIGHT, XK_Left},
	//{PAD_L_DOWN, XK_Down},
	//{PAD_L_LEFT, XK_Left},

	//{PAD_R_UP, XK_KP_8},
	//{PAD_R_RIGHT, XK_KP_6},
	//{PAD_R_DOWN, XK_KP_2},
	//{PAD_R_LEFT, XK_KP_4}
};

#endif
#endif
