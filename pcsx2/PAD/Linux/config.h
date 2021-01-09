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

#include "yaml-cpp/yaml.h"


struct pad_options
{
	bool forcefeedback;
	bool reverse_lx;
	bool reverse_ly;
	bool reverse_rx;
	bool reverse_ry;
	bool mouse_l;
	bool mouse_r;

	bool operator==(const pad_options& other) const
	{
		return ((forcefeedback == other.forcefeedback) &&
				(reverse_lx == other.reverse_lx) &&
				(reverse_ly == other.reverse_ly) &&
				(reverse_rx == other.reverse_rx) &&
				(reverse_ry == other.reverse_ry) &&
				(mouse_l == other.mouse_l) &&
				(mouse_r == other.mouse_r));
	}
};

class PADconf
{
public:
	u32 ff_intensity;
	u32 sensibility;

	bool log;
	bool ftw;
	std::array<pad_options, GAMEPAD_NUMBER> options;
	bool multitap[2];
	std::map<u32, u32> keysym_map[GAMEPAD_NUMBER];
	std::array<size_t, GAMEPAD_NUMBER> unique_id;
	std::vector<std::string> sdl2_mapping;

	PADconf() { init(); }

	void init()
	{
		log = 0;
		ftw = 1;

		ff_intensity = 0x7FFF; // set it at max value by default
		sensibility = 100;
		for (int i = 0; i < GAMEPAD_NUMBER; i++)
		{
			keysym_map[i].clear();
			options[i].forcefeedback = false;
			options[i].reverse_lx = false;
			options[i].reverse_ly = false;
			options[i].reverse_rx = false;
			options[i].reverse_ry = false;
			options[i].mouse_l = false;
			options[i].mouse_r = false;
		}
		unique_id.fill(0);
		sdl2_mapping.clear();
		multitap[0] = false;
		multitap[1] = false;
	}

	void set_joy_uid(u32 pad, size_t uid)
	{
		if (pad < GAMEPAD_NUMBER)
			unique_id[pad] = uid;
	}

	size_t get_joy_uid(u32 pad)
	{
		if (pad < GAMEPAD_NUMBER)
			return unique_id[pad];
		else
			return 0;
	}

	/**
	 * Return (a copy of) private memner ff_instensity
	 **/
	u32 get_ff_intensity()
	{
		return ff_intensity;
	}

	/**
	 * Set intensity while checking that the new value is within
	 * valid range, more than 0x7FFF will cause pad not to rumble(and less than 0 is obviously bad)
	 **/
	void set_ff_intensity(u32 new_intensity)
	{
		if (new_intensity <= 0x7FFF)
		{
			ff_intensity = new_intensity;
		}
	}

	/**
	 * Set sensibility value.
	 * There will be an upper range, and less than 0 is obviously wrong.
	 * We are doing object oriented code, so members are definitely not supposed to be public.
	 **/
	void set_sensibility(u32 new_sensibility)
	{
		if (new_sensibility > 0)
		{
			sensibility = new_sensibility;
		}
		else
		{
			sensibility = 1;
		}
	}

	u32 get_sensibility()
	{
		return sensibility;
	}

	void set_keyboard_key(int pad, int keysym, int index)
	{
		keysym_map[pad][keysym] = index;
	}

	int get_keyboard_key(int pad, int keysym)
	{
		// You must use find instead of []
		// [] will create an element if the key does not exist and return 0
		std::map<u32, u32>::iterator it = keysym_map[pad].find(keysym);
		if (it != keysym_map[pad].end())
			return it->second;
		else
			return -1;
	}
};
extern PADconf g_conf;

extern void ClearKeyboardValues();
extern void DefaultKeyboardValues();
