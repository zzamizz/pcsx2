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

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

#include "AppCoreThread.h"
#include "Utilities/pxStreams.h"

#include "keyboard.h"
#include "PAD.h"
#include "state_management.h"
#include "wx_dialog/dialog.h"

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef SDL_BUILD
#include "SDL.h"
#endif

const u32 revision = 3;
const u32 build = 0; // increase that with each version
#define PAD_SAVE_STATE_VERSION ((revision << 8) | (build << 0))

PADconf g_conf;
keyEvent event;

static keyEvent s_event;

KeyStatus g_key_status;

MtQueue<keyEvent> g_ev_fifo;

#ifndef __APPLE__
Display* GSdsp;
Window GSwin;
#endif

s32 _PADopen(void* pDsp)
{
#ifndef __APPLE__
	GSdsp = *(Display**)pDsp;
	GSwin = (Window) * (((u32*)pDsp) + 1);
#endif

	return 0;
}

void _PADclose()
{
	device_manager->devices.clear();
}

s32 PADinit()
{
	PADLoadConfig();

	Pad::reset_all();

	query.reset();

	for (int port = 0; port < 2; port++)
		slots[port] = 0;

	return 0;
}

void PADshutdown()
{
}

s32 PADopen(void* pDsp)
{
	memset(&event, 0, sizeof(event));
	g_key_status.Init();

	g_ev_fifo.reset();

#if defined(__unix__) || defined(__APPLE__)
	EnumerateDevices();
#endif
	return _PADopen(pDsp);
}

void PADsetLogDir(const char* dir)
{
}

void PADclose()
{
	_PADclose();
}

u32 PADquery()
{
	return 3; // both
}

s32 PADsetSlot(u8 port, u8 slot)
{
	port--;
	slot--;
	if (port > 1 || slot > 3)
	{
		return 0;
	}
	// Even if no pad there, record the slot, as it is the active slot regardless.
	slots[port] = slot;

	return 1;
}

s32 PADfreeze(int mode, freezeData* data)
{
	if (!data)
		return -1;

	if (mode == FREEZE_SIZE)
	{
		data->size = sizeof(PadPluginFreezeData);
	}
	else if (mode == FREEZE_LOAD)
	{
		PadPluginFreezeData* pdata = (PadPluginFreezeData*)(data->data);

		Pad::stop_vibrate_all();

		if (data->size != sizeof(PadPluginFreezeData) || pdata->version != PAD_SAVE_STATE_VERSION ||
			strncmp(pdata->format, "LinPad", sizeof(pdata->format)))
			return 0;

		query = pdata->query;
		if (pdata->query.slot < 4)
		{
			query = pdata->query;
		}

		// Tales of the Abyss - pad fix
		// - restore data for both ports
		for (int port = 0; port < 2; port++)
		{
			for (int slot = 0; slot < 4; slot++)
			{
				u8 mode = pdata->padData[port][slot].mode;

				if (mode != MODE_DIGITAL && mode != MODE_ANALOG && mode != MODE_DS2_NATIVE)
				{
					break;
				}

				memcpy(&pads[port][slot], &pdata->padData[port][slot], sizeof(PadFreezeData));
			}

			if (pdata->slot[port] < 4)
				slots[port] = pdata->slot[port];
		}
	}
	else if (mode == FREEZE_SAVE)
	{
		if (data->size != sizeof(PadPluginFreezeData))
			return 0;

		PadPluginFreezeData* pdata = (PadPluginFreezeData*)(data->data);

		// Tales of the Abyss - pad fix
		// - PCSX2 only saves port0 (save #1), then port1 (save #2)

		memset(pdata, 0, data->size);
		strncpy(pdata->format, "LinPad", sizeof(pdata->format));
		pdata->version = PAD_SAVE_STATE_VERSION;
		pdata->query = query;

		for (int port = 0; port < 2; port++)
		{
			for (int slot = 0; slot < 4; slot++)
			{
				pdata->padData[port][slot] = pads[port][slot];
			}

			pdata->slot[port] = slots[port];
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

u8 PADstartPoll(int pad)
{
	return pad_start_poll(pad);
}

u8 PADpoll(u8 value)
{
	return pad_poll(value);
}

// PADkeyEvent is called every vsync (return NULL if no event)
keyEvent* PADkeyEvent()
{
#ifdef SDL_BUILD
	// Take the opportunity to handle hot plugging here
	SDL_Event events;
	while (SDL_PollEvent(&events))
	{
		switch (events.type)
		{
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
				EnumerateDevices();
				break;
			default:
				break;
		}
	}
#endif
#ifdef __unix__
	if (g_ev_fifo.size() == 0)
	{
		// PAD_LOG("No events in queue, returning empty event\n");
		s_event = event;
		event.evt = 0;
		event.key = 0;
		return &s_event;
	}
	s_event = g_ev_fifo.dequeue();

	AnalyzeKeyEvent(s_event);
	// PAD_LOG("Returning Event. Event Type: %d, Key: %d\n", s_event.evt, s_event.key);
	return &s_event;
#else // MacOS
    s_event = event;
    event.evt = 0;
    event.key = 0;
    return &s_event;
#endif
}

#if defined(__unix__)
void PADWriteEvent(keyEvent& evt)
{
	// if (evt.evt != 6) { // Skip mouse move events for logging
	//     PAD_LOG("Pushing Event. Event Type: %d, Key: %d\n", evt.evt, evt.key);
	// }
	g_ev_fifo.push(evt);
}
#endif

void PADDoFreezeOut(void* dest)
{
	freezeData fP = {0, (s8*)dest};
	if (PADfreeze(FREEZE_SIZE, &fP) != 0)
		return;
	if (!fP.size)
		return;

	Console.Indent().WriteLn("Saving PAD");

	if (PADfreeze(FREEZE_SAVE, &fP) != 0)
		throw std::runtime_error(" * PAD: Error saving state!\n");
}


void PADDoFreezeIn(pxInputStream& infp)
{
	freezeData fP = {0, nullptr};
	if (PADfreeze(FREEZE_SIZE, &fP) != 0)
		fP.size = 0;

	Console.Indent().WriteLn("Loading PAD");

	if (!infp.IsOk() || !infp.Length())
	{
		// no state data to read, but PAD expects some state data?
		// Issue a warning to console...
		if (fP.size != 0)
			Console.Indent().Warning("Warning: No data for PAD found. Status may be unpredictable.");

		return;

		// Note: Size mismatch check could also be done here on loading, but
		// some plugins may have built-in version support for non-native formats or
		// older versions of a different size... or could give different sizes depending
		// on the status of the plugin when loading, so let's ignore it.
	}

	ScopedAlloc<s8> data(fP.size);
	fP.data = data.GetPtr();

	infp.Read(fP.data, fP.size);
	if (PADfreeze(FREEZE_LOAD, &fP) != 0)
		throw std::runtime_error(" * PAD: Error loading state!\n");
}

void PollForJoystickInput(int cpad)
{
	int index = Device::uid_to_index(cpad);
	if (index < 0)
		return;

	auto& gamePad = device_manager->devices[index];

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

// Actually PADupdate is always call with pad == 0. So you need to update both
// pads -- Gregory
void PADupdate(int pad)
{
#ifndef __APPLE__
	// Gamepad inputs don't count as an activity. Therefore screensaver will
	// be fired after a couple of minute.
	// Emulate an user activity
	static int count = 0;
	count++;
	if ((count & 0xFFF) == 0)
	{
		// 1 call every 4096 Vsync is enough
		XResetScreenSaver(GSdsp);
	}
#endif

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

void PADconfigure()
{
	ScopedCoreThreadPause paused_core;
	PADLoadConfig();

	DisplayDialog();
	paused_core.AllowResume();
	return;
}
