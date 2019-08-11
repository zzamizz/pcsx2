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

#include "twopad.h"
#include "ps2_pad.h"

void ps2_pad::Init()
{
        m_state_access = false;
        
        m_button = 0xFFFF;
        m_internal_button_kbd = 0xFFFF;
        m_internal_button_joy = 0xFFFF;

        for (int index = 0; index < MAX_KEYS; index++)
        {
            m_button_pressure[index] = 0xFF;
            m_internal_button_pressure[index] = 0xFF;
        }

        m_analog.init();
        m_internal_analog_kbd.init();
        m_internal_analog_joy.init();
}

void ps2_pad::press( u32 index, s32 value)
{
    if (!IsAnalogKey(index))
    {
        m_internal_button_pressure[index] = value;

        if (m_state_access)
            clear_bit(m_internal_button_kbd, index);
        else
            clear_bit(m_internal_button_joy, index);
    }
    else
    {
        // clamp value
        if (value > MAX_ANALOG_VALUE)
            value = MAX_ANALOG_VALUE;
        else if (value < -MAX_ANALOG_VALUE)
            value = -MAX_ANALOG_VALUE;

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
        if (m_state_access)
            set_bit(m_internal_button_kbd, index);
        else
            set_bit(m_internal_button_joy, index);
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
    return m_button;
}

void ps2_pad::analog_set(u32 index, u8 value)
{
    PADAnalog *m_internal_analog_ref;

    if (m_state_access)
        m_internal_analog_ref = &m_internal_analog_kbd;
    else
        m_internal_analog_ref = &m_internal_analog_joy;

    switch (index)
    {
        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            m_internal_analog_ref->rx = value;
            break;

        case PAD_R_DOWN:
        case PAD_R_UP:
            m_internal_analog_ref->ry = value;
            break;

        case PAD_L_LEFT:
        case PAD_L_RIGHT:
            m_internal_analog_ref->lx = value;
            break;

        case PAD_L_DOWN:
        case PAD_L_UP:
            m_internal_analog_ref->ly = value;
            break;

        default:
            break;
    }
}

bool ps2_pad::analog_is_reversed(u32 index)
{
    /*switch (index)
    {
        case PAD_L_RIGHT:
        case PAD_L_LEFT:
            return (conf->pad_options.reverse_lx);

        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            return (conf->pad_options.reverse_rx);

        case PAD_L_UP:
        case PAD_L_DOWN:
            return (conf->pad_options.reverse_ly);

        case PAD_R_DOWN:
        case PAD_R_UP:
            return (conf->pad_options.reverse_ry);

        default:
            return false;
    }*/
    //return false;
    return false;
}

u8 ps2_pad::get(u32 index)
{
    switch (index)
    {
        case PAD_R_LEFT:
        case PAD_R_RIGHT:
            return m_analog.rx;

        case PAD_R_DOWN:
        case PAD_R_UP:
            return m_analog.ry;

        case PAD_L_LEFT:
        case PAD_L_RIGHT:
            return m_analog.lx;

        case PAD_L_DOWN:
        case PAD_L_UP:
            return m_analog.ly;

        default:
            return m_button_pressure[index];
    }
}

u8 ps2_pad::get_result(u16 value, u32 test)
{
    return !test_bit(value, test) ? get(test) : 0;
}

u8 ps2_pad::analog_merge(u8 kbd, u8 joy)
{
    if (kbd != m_analog_released_val)
        return kbd;
    else
        return joy;
}

void ps2_pad::commit_status()
{
    m_button = m_internal_button_kbd & m_internal_button_joy;

    for (int index = 0; index < MAX_KEYS; index++)
        m_button_pressure[index] = m_internal_button_pressure[index];

    m_analog.lx = analog_merge(m_internal_analog_kbd.lx, m_internal_analog_joy.lx);
    m_analog.ly = analog_merge(m_internal_analog_kbd.ly, m_internal_analog_joy.ly);
    m_analog.rx = analog_merge(m_internal_analog_kbd.rx, m_internal_analog_joy.rx);
    m_analog.ry = analog_merge(m_internal_analog_kbd.ry, m_internal_analog_joy.ry);
}
