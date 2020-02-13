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

#include "twopad.h"
#include "ps2_pad.h"
#include <algorithm>

void ps2_pad::Init()
{
    joystick_state_access();
        
    main.init();
    kbd.init();
    joy.init();

    m_button_pressure.fill(0xFF);
    m_internal_button_pressure.fill(0xFF);
}

void ps2_pad::press( u32 index, s32 value)
{
    if (!IsAnalogKey(index))
    {
        m_internal_button_pressure[index] = value;

        clear_bit(current()->button, index);
    }
    else
    {
        value = clip(value, -MAX_ANALOG_VALUE, MAX_ANALOG_VALUE);

        //                          Left -> -- -> Right
        // Value range :        FFFF8002 -> 0  -> 7FFE
        // Force range :			  80 -> 0  -> 7F
        // Normal mode : expect value 0  -> 80 -> FF
        // Reverse mode: expect value FF -> 7F -> 0
        u8 force = (value / 256);

        if (analog_is_reversed(index))
            analog_set(index, m_analog_released_val - force);
        else
            analog_set(index, m_analog_released_val + force);
    }
}

void ps2_pad::release(u32 index)
{
    if (IsAnalogKey(index))
    {
        analog_set(index, m_analog_released_val);
    }
    else
    {
        set_bit(current()->button, index);
    }
}

void ps2_pad::set(u32 index, s32 value)
{
    if (value != 0)
        press(index, value);
    else
        release(index);
}

u16 ps2_pad::get()
{
    return main.button;
}

void ps2_pad::analog_set(u32 index, u8 value)
{
    switch (index)
    {
        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            current()->analog.rx = value;
            break;

        case PAD_R_DOWN:
        case PAD_R_UP:
            current()->analog.ry = value;
            break;

        case PAD_L_LEFT:
        case PAD_L_RIGHT:
            current()->analog.lx = value;
            break;

        case PAD_L_DOWN:
        case PAD_L_UP:
            current()->analog.ly = value;
            break;

        default:
            break;
    }
}

bool ps2_pad::analog_is_reversed(u32 index)
{
    switch (index)
    {
        case PAD_L_RIGHT:
        case PAD_L_LEFT:
            return (reversed_lx);

        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            return (reversed_rx);

        case PAD_L_UP:
        case PAD_L_DOWN:
            return (reversed_ly);

        case PAD_R_DOWN:
        case PAD_R_UP:
            return (reversed_ry);

        default:
            return false;
    }
    return false;
}

u8 ps2_pad::get(u32 index)
{
    switch (index)
    {
        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            return main.analog.rx;

        case PAD_R_DOWN:
        case PAD_R_UP:
            return main.analog.ry;

        case PAD_L_LEFT:
        case PAD_L_RIGHT:
            return main.analog.lx;

        case PAD_L_DOWN:
        case PAD_L_UP:
            return main.analog.ly;

        default:
            return m_button_pressure[index];
    }
}

u8 ps2_pad::get_result(u16 value, u32 test)
{
    return !test_bit(value, test) ? get(test) : 0;
}

void ps2_pad::commit_status()
{
    main.button = kbd.button & joy.button;

    m_button_pressure = m_internal_button_pressure;

    main.analog.merge(kbd.analog, joy.analog);
}
