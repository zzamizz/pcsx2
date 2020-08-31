/*  TwoPAD - author: arcum42(@gmail.com)
 *  Copyright (C) 2019
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

#include "twopad.h"
#include <array>

static const u32 MAX_ANALOG_VALUE = 32766;
static const u32 m_analog_released_val = 0x7F;

class PADAnalog
{
    public:
        u8 lx, ly;
        u8 rx, ry;

        PADAnalog()
        {
            init();
        }

        void init()
        {
            lx = m_analog_released_val;
            ly = m_analog_released_val;
            rx = m_analog_released_val;
            ry = m_analog_released_val;
        }
};

class ps2_pad
{
    private:
        u16 m_button;
        u16 m_internal_button_kbd;
        u16 m_internal_button_joy;

        std::array<u8, MAX_KEYS> m_button_pressure;
        std::array<u8, MAX_KEYS> m_internal_button_pressure;

        bool m_state_access;

        PADAnalog m_analog;
        PADAnalog m_internal_analog_kbd;
        PADAnalog m_internal_analog_joy;

        void analog_set(u32 index, u8 value);
        bool analog_is_reversed(u32 index);
        u8 analog_merge(u8 kbd, u8 joy);

    public:
        ps2_pad()
        {
            Init();
        }
        void Init();

        void keyboard_state_access() { m_state_access = true; }
        void joystick_state_access() { m_state_access = false; }

        void press(u32 index, s32 value = 0xFF);
        void release( u32 index);

        u16 get();
        u8 get(u32 index);
        u8 get_result(u16 value, u32 test);

        void commit_status();
};