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

#pragma once

#ifndef __KEYSTATUS_H__
#define __KEYSTATUS_H__

#include "Global.h"
#include "config.h"

typedef struct
{
	u8 lx, ly;
	u8 rx, ry;
	
	void set(u8 lx2, u8 ly2, u8 rx2, u8 ry2)
	{
		lx = lx2;
		ly = ly2;
		rx = rx2;
		ry = ry2;
	}

	void set(u8 value)
	{
		lx = value;
		ly = value;
		rx = value;
		ry = value;
	}
} PADAnalog;

#define MAX_ANALOG_VALUE 32766

class KeyStatus
{
private:
	const u8 m_analog_released_val;

	u8 m_pad;
	u16 m_button;
	std::array<u16, 2> m_internal_button;

	std::array<u8, MAX_KEYS> m_button_pressure;
	std::array<u8, MAX_KEYS> m_internal_button_pressure;

	u8 m_state_acces;

	PADAnalog m_analog;
	std::array<PADAnalog, 2> m_internal_analog;

	void analog_set(u32 index, u8 value);
	bool analog_is_reversed(u32 index);
	u8 analog_merge(u8 kbd, u8 joy);

public:
	KeyStatus(u8 pad = 0)
		: m_analog_released_val(0x7F)
	{
		Init(pad);
	}
	void Init(u8 pad);

	void keyboard_state_acces() { m_state_acces = 0; }
	void joystick_state_acces() { m_state_acces = 1; }

	void press(u32 index, s32 value = 0xFF);
	void press_button(u32 index);
	void release( u32 index);

	u16 get();
	u8 get(u32 index);


	void commit_status();
};

extern std::array<KeyStatus, GAMEPAD_NUMBER> g_key_status;

#endif
