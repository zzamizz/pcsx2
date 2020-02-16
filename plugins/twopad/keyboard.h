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

#if defined(__unix__)
#include "linux/keyboard_x11.h"
#endif

class keyboard_control
{
    private:
    public:
        keyboard_control();
        ~keyboard_control();

        bool auto_repeat = false;

        void poll_keyboard();
        void set_autorepeat(bool repeat);
        void idle();
        const char* control_to_string(int cpad, int key);
        void init_keys();

        #if defined(__unix__)
        const char* key_to_string(KeySym k);
        bool get_key_mouse_event(u32 &pkey);
        KeySym get_key(int cpad, int key);
        void set_key(int cpad, int key, KeySym val);
        #endif
};

extern keyboard_control *keys;