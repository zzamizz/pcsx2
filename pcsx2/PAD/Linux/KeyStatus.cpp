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

#include "KeyStatus.h"

void KeyStatus::Init(u8 pad)
{
	m_pad = pad;
	m_button = 0xFFFF;
	m_internal_button[0] = 0xFFFF;
	m_internal_button[1] = 0xFFFF;
	m_state_index = 0;

	for (auto& key : all_keys)
	{
		m_button_pressure[(int)key] = 0xFF;
		m_internal_button_pressure[(int)key] = 0xFF;
	}

	m_analog.set(m_analog_released_val);
	m_internal_analog[0].set(m_analog_released_val);
	m_internal_analog[1].set(m_analog_released_val);
}

void KeyStatus::press(u32 index, s32 value)
{
	if (!IsAnalogKey(index))
	{
		m_internal_button_pressure[index] = value;
		clear_bit(m_internal_button[m_state_index], index);
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

///  but with proper handling for analog buttons
void KeyStatus::press_button(u32 button)
{
	// Analog controls.
	if (IsAnalogKey(button))
	{
		switch (button)
		{
			case PAD_R_LEFT:
			case PAD_R_UP:
			case PAD_L_LEFT:
			case PAD_L_UP:
				press(button, -MAX_ANALOG_VALUE);
				break;
			case PAD_R_RIGHT:
			case PAD_R_DOWN:
			case PAD_L_RIGHT:
			case PAD_L_DOWN:
				press(button, MAX_ANALOG_VALUE);
				break;
		}
	}
	else
	{
		press(button);
	}
}

void KeyStatus::release(u32 index)
{
	if (!IsAnalogKey(index))
	{
		set_bit(m_internal_button[m_state_index], index);
	}
	else
	{
		analog_set(index, m_analog_released_val);
	}
}

u16 KeyStatus::get()
{
	return m_button;
}

void KeyStatus::analog_set(u32 index, u8 value)
{
	PADAnalog* m_internal_analog_ref;
	m_internal_analog_ref = &m_internal_analog[m_state_index];

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

bool KeyStatus::analog_is_reversed(u32 index)
{
	switch (index)
	{
		case PAD_L_RIGHT:
		case PAD_L_LEFT:
			return (g_conf.options[m_pad].reverse_lx);

		case PAD_R_LEFT:
		case PAD_R_RIGHT:
			return (g_conf.options[m_pad].reverse_rx);

		case PAD_L_UP:
		case PAD_L_DOWN:
			return (g_conf.options[m_pad].reverse_ly);

		case PAD_R_DOWN:
		case PAD_R_UP:
			return (g_conf.options[m_pad].reverse_ry);

		default:
			return false;
	}
}

u8 KeyStatus::get(u32 index)
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

u8 KeyStatus::analog_merge(u8 kbd, u8 joy)
{
	if (kbd != m_analog_released_val)
		return kbd;
	else
		return joy;
}

void KeyStatus::commit_status()
{
	m_button = m_internal_button[0] & m_internal_button[1];

	for (auto& key : all_keys)
		m_button_pressure[(int)key] = m_internal_button_pressure[(int)key];

	m_analog.lx = analog_merge(m_internal_analog[0].lx, m_internal_analog[1].lx);
	m_analog.ly = analog_merge(m_internal_analog[0].ly, m_internal_analog[1].ly);
	m_analog.rx = analog_merge(m_internal_analog[0].rx, m_internal_analog[1].rx);
	m_analog.ry = analog_merge(m_internal_analog[0].ry, m_internal_analog[1].ry);
}
