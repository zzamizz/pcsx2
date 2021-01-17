/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021  PCSX2 Dev Team
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

[[maybe_unused]]static const u8 MODE_PS1_MOUSE = 0x12;
[[maybe_unused]]static const u8 MODE_NEGCON = 0x23;
[[maybe_unused]]static const u8 MODE_DIGITAL = 0x41;
[[maybe_unused]]static const u8 MODE_ANALOG = 0x73;
[[maybe_unused]]static const u8 MODE_DS2_NATIVE = 0x79;


// Typical packet response on the bus
[[maybe_unused]]static const u8 ConfigExit[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//static const u8 ConfigExit[7] = {0x5A, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

[[maybe_unused]]static const u8 noclue[7] = {0x5A, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5A};
[[maybe_unused]]static u8 queryMaskMode[7] = {0x5A, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x5A};
//static const u8 DSNonNativeMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
[[maybe_unused]]static const u8 setMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// DS2
[[maybe_unused]]static const u8 queryModelDS2[7] = {0x5A, 0x03, 0x02, 0x00, 0x02, 0x01, 0x00};
// DS1
[[maybe_unused]]static const u8 queryModelDS1[7] = {0x5A, 0x01, 0x02, 0x00, 0x02, 0x01, 0x00};

[[maybe_unused]]static const u8 queryAct[2][7] = {{0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A},
								  {0x5A, 0x00, 0x00, 0x01, 0x01, 0x01, 0x14}};

[[maybe_unused]]static const u8 queryComb[7] = {0x5A, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00};

[[maybe_unused]]static const u8 queryMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


[[maybe_unused]]static const u8 setNativeMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A};


// Freeze data, for a single pad.  Basically has all pad state that
// a PS2 can set.
struct PadFreezeData
{
	// Digital / Analog / DS2 Native
	u8 mode;

#ifdef _MSC_VER
	u8 previousType;
#endif

	u8 modeLock;

	// In config mode
	u8 config;

	u8 vibrate[8];
	u8 umask[2];

	// Vibration indices.
	u8 vibrateI[2];

	// Last vibration value sent to controller.
	// Only used so as not to call vibration
	// functions when old and new values are both 0.
	u8 currentVibrate[2];

	// Next vibrate val to send to controller.  If next and current are
	// both 0, nothing is sent to the controller.  Otherwise, it's sent
	// on every update.
	u8 nextVibrate[2];
};

// The state of the PS2 bus
struct QueryInfo
{
	u8 port;
	u8 slot;
	u8 lastByte;
	u8 currentCommand;
	u8 numBytes;
	u8 queryDone;
	u8 response[42];

	void reset();
	u8 start_poll(int port);

#ifndef _MSC_VER
	template <size_t S>
	void set_result(const u8 (&rsp)[S])
	{
		memcpy(response + 2, rsp, S);
		numBytes = 2 + S;
	}

	template <size_t S>
	void set_final_result(const u8 (&rsp)[S])
	{
		set_result(rsp);
		queryDone = 1;
	}
#endif
};

extern QueryInfo query;

// Full state to manage save state
struct PadPluginFreezeData
{
	char format[8];
	// Currently all different versions are incompatible.
	// May split into major/minor with some compatibility rules.
	u32 version;

#ifdef _MSC_VER
	// So when loading, know which plugin's settings I'm loading.
	// Not a big deal.  Use a static variable when saving to figure it out.
	u8 port;
#endif

	// active slot for port
	u8 slot[2];
	PadFreezeData padData[2][4];
	QueryInfo query = {0, 0, 0, 0, 0, 0xFF, {0xF3}};
};

#ifdef _MSC_VER
class Pad : public PadFreezeData
{
public:
	// Current pad state.
	ButtonSum sum;

	// State of locked buttons.  Already included by sum, used
	// as initial value of sum.
	ButtonSum lockedSum;

	// Flags for which controls (buttons or axes) are locked, if any.
	DWORD lockedState;

	// Used to keep track of which pads I'm running.
	// Note that initialized pads *can* be disabled.
	// I keep track of state of non-disabled non-initialized
	// pads, but should never be asked for their state.
	u8 initialized;

	// Set to 1 if the state of this pad has been updated since its state
	// was last queried.
	char stateUpdated;

	// initialized and not disabled (and mtap state for slots > 0).
	u8 enabled;
};
#else
class Pad : public PadFreezeData
{
public:
	// Lilypad store here the state of PC pad

	void rumble(unsigned port);
	void set_vibrate(int motor, u8 val);
	void reset_vibrate();
	void reset();

	void set_mode(int mode);

	static void reset_all();
	static void stop_vibrate_all();
	static void rumble_all();
};
#endif
