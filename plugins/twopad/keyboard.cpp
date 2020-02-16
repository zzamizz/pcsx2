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

#include "keyboard.h"

keyboard_control *keys;

keyboard_control::keyboard_control()
{
    #if defined(__unix__)
    init_x11_keys();
    #endif
}

keyboard_control::~keyboard_control()
{
    #if defined(__unix__)
    #endif
    set_autorepeat(true);
}

void keyboard_control::poll_keyboard()
{
    #if defined(__unix__)
    PollForX11KeyboardInput();
    #endif
}

void keyboard_control::set_autorepeat(bool repeat)
{
    if (auto_repeat)
    {
        #if defined(__unix__)
        SetAutoRepeat(repeat);
        #endif
    }
}

void keyboard_control::idle()
{
#if defined(__unix__)
    // Gamepad inputs don't count as an activity. Therefore screensaver will
    // be fired after a couple of minutes.
    // Emulate an user activity.
    static int count = 0;
    count++;

    // 1 call every 4096 Vsync is enough.
    if ((count & 0xFFF) == 0) 
    {
        XResetScreenSaver(GSdsp);
    }
#endif
}

const char* keyboard_control::control_to_string(int cpad, int key)
{
    return key_to_string(get_key(cpad, key));
}

#if defined(__unix__)
bool keyboard_control::get_key_mouse_event(u32 &pkey)
{
    return PollX11KeyboardMouseEvent(pkey);
}

KeySym keyboard_control::get_key(int cpad, int key)
{
    return x11_key_map[cpad][key];
}

void keyboard_control::set_key(int cpad, int key, KeySym val)
{
    x11_key_map[cpad][key] = val;
}

const char* keyboard_control::key_to_string(KeySym k)
{
    if (k <= 0) return "<not set>";

    return KeyToString(k);
}
#endif