/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
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

#include "Plugins.h"
#include "MemoryCardFile.h"

#define MODE_PS1_MOUSE 0x12
#define MODE_NEGCON 0x23
#define MODE_DIGITAL 0x41
#define MODE_ANALOG 0x73
#define MODE_DS2_NATIVE 0x79

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

#ifndef _MSC_VER
	void reset();
	u8 start_poll(int port);

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

struct _mcd
{
	u8 term; // terminator value;

	bool goodSector; // xor sector check
	u32 sectorAddr;  // read/write sector address
	u32 transferAddr; // Transfer address

	u8 FLAG;  // for PSX;
	
	u8 port; // port 
	u8 slot; // and slot for this memcard

	// Auto Eject
	u32 ForceEjection_Timeout; // in SIO checks
	wxDateTime ForceEjection_Timestamp;


	void GetSizeInfo(PS2E_McdSizeInfo &info)
	{
		SysPlugins.McdGetSizeInfo(port, slot, info);
	}

	bool IsPSX()
	{
		return SysPlugins.McdIsPSX(port, slot);
	}

	void EraseBlock()
	{
		SysPlugins.McdEraseBlock(port, slot, transferAddr);
	}

	// Read from memorycard to dest
	void Read(u8 *dest, int size) 
	{
		SysPlugins.McdRead(port, slot, dest, transferAddr, size);
	}

	// Write to memorycard from src
	void Write(u8 *src, int size) 
	{
		SysPlugins.McdSave(port, slot, src,transferAddr, size);
	}

	bool IsPresent()
	{
		return SysPlugins.McdIsPresent(port, slot);
	}

	u8 DoXor(const u8 *buf, uint length)
	{
		u8 i, x;
		for (x=0, i=0; i<length; i++) x ^= buf[i];
		return x;
	}

	u64 GetChecksum()
	{
		return SysPlugins.McdGetCRC(port, slot);
	}

	void NextFrame() {
		SysPlugins.McdNextFrame( port, slot );
	}

	bool ReIndex(const wxString& filter = L"") {
		return SysPlugins.McdReIndex( port, slot, filter );
	}
};

struct _sio
{
	u16 StatReg;
	u16 ModeReg;
	u16 CtrlReg;
	u16 BaudReg;

	u32 count;     // old_sio remnant
	u32 packetsize;// old_sio remnant

	u8 buf[512];
	u8 ret; // default return value;
	u8 cmd; // command backup

	u16 bufCount; // current buffer counter
	u16 bufSize;  // supposed buffer size

	u8 port;    // current port
	u8 slot[2]; // current slot

	u8 GetPort() { return port; }
	u8 GetSlot() { return slot[port]; }
};

extern _sio sio;
extern _mcd mcds[2][4];
extern _mcd *mcd;

extern void sioInit();
extern u8 sioRead8();
extern void sioWrite8(u8 value);
extern void sioWriteCtrl16(u16 value);
extern void sioInterrupt();
extern void sioInterruptR();
extern void SetForceMcdEjectTimeoutNow();
extern void ClearMcdEjectTimeoutNow();
extern void sioStatRead();
extern void sioSetGameSerial(const wxString& serial);
extern void sioNextFrame();
