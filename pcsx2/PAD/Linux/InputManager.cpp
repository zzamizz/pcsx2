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

#include "Device.h"
#include "InputManager.h"
#include "keyboard.h"
#include "state_management.h"

#ifdef SDL_BUILD
#include "Devices/SDL2Gamepad.h"
#endif

std::unique_ptr<InputDeviceManager> device_manager(new InputDeviceManager);

InputDeviceManager::InputDeviceManager()
{
}

InputDeviceManager::~InputDeviceManager()
{
	device_manager->devices.clear();
}

void InputDeviceManager::PollForJoystickInput(int cpad)
{
	int index = Device::uid_to_index(cpad);
	if (index < 0)
		return;

	auto& gamePad = devices[index];

	gamePad->UpdateDeviceState();

	for (int i = 0; i < MAX_KEYS; i++)
	{
		s32 value = gamePad->GetInput((gamePadValues)i);
		if (value != 0)
			g_key_status.press(cpad, i, value);
		else
			g_key_status.release(cpad, i);
	}
}

void InputDeviceManager::Update()
{
    // Poll keyboard/mouse event. There is currently no way to separate pad0 from pad1 event.
	// So we will populate both pad in the same time
	for (int cpad = 0; cpad < GAMEPAD_NUMBER; cpad++)
	{
		g_key_status.keyboard_state_acces(cpad);
	}
	UpdateKeyboardInput();

	// Get joystick state + Commit
	for (int cpad = 0; cpad < GAMEPAD_NUMBER; cpad++)
	{
		g_key_status.joystick_state_acces(cpad);

		PollForJoystickInput(cpad);

		g_key_status.commit_status(cpad);
	}

	Pad::rumble_all();
}

/**
 * Find every interesting devices and create right structure for them(depend on backend)
 **/
void EnumerateDevices()
{
#ifdef SDL_BUILD
	SDL2Gamepad::EnumerateJoysticks();
#endif
}
