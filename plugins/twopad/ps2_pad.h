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

#include <array>
#include <algorithm>

#include "twopad.h"
#include "sdl_controller.h"

static const s32 MAX_ANALOG_VALUE = 32766;
static const s32 m_analog_released_val = 0x7F;

// From https://stackoverflow.com/questions/9323903/most-efficient-elegant-way-to-clip-a-number
// because std::clamp is C++17.
template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

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

        void merge(PADAnalog temp, PADAnalog temp2)
        {
            if (temp.lx != m_analog_released_val)
                lx = temp.lx;
            else
                lx = temp2.lx;
            
            
            if (temp.ly != m_analog_released_val)
                ly = temp.ly;
            else
                ly = temp2.ly;

            if (temp.rx != m_analog_released_val)
                rx = temp.rx;
            else
                rx = temp2.rx;

            if (temp.ry != m_analog_released_val)
                ry = temp.ry;
            else
                ry = temp2.ry;
        }
};

class ps2_pad_internal
{
    public:
        u16 button;
        PADAnalog analog;

        ps2_pad_internal() { init(); }

        void init()
        {
            button = 0xFFFF;
            analog.init();
        }
};

class ps2_pad
{
    private:
        // Toggle keyboard/joystick. keyboard = true.
        bool m_state_access;

        ps2_pad_internal kbd, joy, main;

        void analog_set(u32 index, u8 value);
        bool analog_is_reversed(u32 index);

        std::array<u8, MAX_KEYS> m_button_pressure;
        std::array<u8, MAX_KEYS> m_internal_button_pressure;
        
    public:
        ps2_pad() { Init();}

        void Init();

        void keyboard_state_access() { m_state_access = true; }
        void joystick_state_access() { m_state_access = false; }

        ps2_pad_internal *current()
        {
            if (m_state_access)
                return &kbd;
            else
                return &joy;
        }

        void press(u32 index, s32 value = 0xFF);
        void release( u32 index);

        u16 get();
        u8 get(u32 index);
        u8 get_result(u16 value, u32 test);
        void set(u32 index, s32 value);

        void commit_status();

        bool reversed_lx = false;
        bool reversed_ly = false;
        bool reversed_rx = false;
        bool reversed_ry = false;

        sdl_controller *real = nullptr;
        bool controller_attached = false;
};

extern std::array<ps2_pad, 2>ps2_gamepad;