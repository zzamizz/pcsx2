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

#include <SDL.h>
#include <SDL_haptic.h>

#include "../Device.h"
#include "../PAD.h"
#include "../config.h"

#define NB_EFFECT 2 // Don't use more than two, ps2 only has one for big motor and one for small(like most systems)
// holds all joystick info
class SDL2Gamepad : public Device
{
public:
	SDL2Gamepad(int id);
	~SDL2Gamepad();

	SDL2Gamepad(const SDL2Gamepad&) = delete;            // copy constructor
	SDL2Gamepad& operator=(const SDL2Gamepad&) = delete; // assignment


	// opens handles to all possible joysticks
	static void EnumerateJoysticks();

	void Rumble(unsigned type, unsigned pad) override;

	bool TestForce(float) override;

	const char* GetName() final;

	int GetInput(gamePadValues input) final;

	const char* GetBindingName(int key) final;

	void UpdateDeviceState() final;

	size_t GetUniqueIdentifier() final;

private:
	SDL_GameController* m_controller;
	SDL_Haptic* m_haptic;
	std::array<int, NB_EFFECT> m_effects_id;
};

[[maybe_unused]]static std::map<int, const char*> sdl2_key_names =
{
	//wxBTN_PAD_ID_MOUSE, 

	{SDL_CONTROLLER_BUTTON_Y, "[Y]"},
	{SDL_CONTROLLER_BUTTON_B, "[B]"},
	{SDL_CONTROLLER_BUTTON_A, "[A]"},
	{SDL_CONTROLLER_BUTTON_X, "[X]"},

	{SDL_CONTROLLER_BUTTON_START, "[Start]"},
	{SDL_CONTROLLER_BUTTON_GUIDE, "[Guide]"},
	{SDL_CONTROLLER_BUTTON_BACK, "[Back]"},

	{SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "[L1]"},
	{SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "[R1]"},
	{SDL_CONTROLLER_BUTTON_LEFTSTICK, "[L3]"},
	{SDL_CONTROLLER_BUTTON_RIGHTSTICK, "[R3]"},

	{SDL_CONTROLLER_AXIS_TRIGGERLEFT, "[L2]"},
	{SDL_CONTROLLER_AXIS_TRIGGERRIGHT, "[R2]"},

	{SDL_CONTROLLER_BUTTON_DPAD_UP, "[D-Pad Up]"},
	{SDL_CONTROLLER_BUTTON_DPAD_RIGHT, "[D-Pad Right]"},
	{SDL_CONTROLLER_BUTTON_DPAD_DOWN, "[D-Pad Down]"},
	{SDL_CONTROLLER_BUTTON_DPAD_LEFT, "[D-Pad Left]"},

	{SDL_CONTROLLER_AXIS_LEFTY, "[Left Y Axis]"},
	{SDL_CONTROLLER_AXIS_LEFTX, "[Left X Axis]"},
	{SDL_CONTROLLER_AXIS_RIGHTY, "[Right Y Axis]"},
	{SDL_CONTROLLER_AXIS_RIGHTX, "[Right X Axis]"},

	{SDL_CONTROLLER_AXIS_INVALID, "[Invalid Axis]"},
	{SDL_CONTROLLER_BUTTON_INVALID, "[Invalid Button]"}
};