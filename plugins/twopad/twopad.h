/*  TwoPAD - author: arcum42(@gmail.com)
 *  Copyright (C) 2019-2020
 *
 *  Based on ZeroPAD, author zerofrog@gmail.com
 *  And OnePAD, author arcum42, gregory, PCSX2 team
 *  Copyright (C) 2006-2007
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#pragma once

#define PADdefs
#include "PS2Edefs.h"
#include "PS2Eext.h"

#include <algorithm>
#include <array>
#include <string>

enum gamePadValues {
    PAD_L2 = 0,   // L2 button
    PAD_R2,       // R2 button
    PAD_L1,       // L1 button
    PAD_R1,       // R1 button
    PAD_TRIANGLE, // Triangle button ▲
    PAD_CIRCLE,   // Circle button ●
    PAD_CROSS,    // Cross button ✖
    PAD_SQUARE,   // Square button ■
    PAD_SELECT,   // Select button
    PAD_L3,       // Left joystick button (L3)
    PAD_R3,       // Right joystick button (R3)
    PAD_START,    // Start button
    PAD_UP,       // Directional pad ↑
    PAD_RIGHT,    // Directional pad →
    PAD_DOWN,     // Directional pad ↓
    PAD_LEFT,     // Directional pad ←
    PAD_L_UP,     // Left joystick (Up) ↑
    PAD_L_RIGHT,  // Left joystick (Right) →
    PAD_L_DOWN,   // Left joystick (Down) ↓
    PAD_L_LEFT,   // Left joystick (Left) ←
    PAD_R_UP,     // Right joystick (Up) ↑
    PAD_R_RIGHT,  // Right joystick (Right) →
    PAD_R_DOWN,   // Right joystick (Down) ↓
    PAD_R_LEFT,    // Right joystick (Left) ←
    PAD_NULL
};

// Number of elements in gamePadValues.
static const u32 MAX_KEYS = (u32)PAD_NULL;


const std::array<std::string, MAX_KEYS> gamepad_names = { 
    "L2",
    "R2",
    "L1",
    "R1",
    "Triangle",
    "Circle",
    "Cross",
    "Square",
    "Select",
    "L3",
    "R3",
    "Start",
    "Pad Up",
    "Pad Right",
    "Pad Down",
    "Pad Left",
   "Left Stick Up",
   "Left Stick Right",
   "Left Stick Down",
   "Left Stick Left",
    "Right Stick Up",
    "Right Stick Right",
    "Right Stick Down",
    "Right Stick Left"
};

static __forceinline bool IsAnalogKey(int index)
{
    return ((index >= PAD_L_UP) && (index <= PAD_R_LEFT));
}
