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

#pragma once

#include "twopad.h"
#include "ps2_pad.h"

// Borrowing a bunch from onepad, which borrowed from Lilypad.

// Typical packet response on the bus
static const u8 configExit[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 noclue[7] = {0x5A, 0x00, 0x00, 0x02, 0x00, 0x00, 0x5A};
static const u8 setMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 queryModelDS2[7] = {0x5A, 0x03, 0x02, 0x00, 0x02, 0x01, 0x00};
static const u8 queryModelDS1[7] = {0x5A, 0x01, 0x02, 0x00, 0x02, 0x01, 0x00};
static const u8 queryComb[7] = {0x5A, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00};
static const u8 queryMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 setNativeMode[7] = {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A};

static u8 queryMaskMode[7] = {0x5A, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x5A};

static const u8 queryAct[2][7] = {
    {0x5A, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0A},
    {0x5A, 0x00, 0x00, 0x01, 0x01, 0x01, 0x14}};

static const u32 MODE_DIGITAL = 0x41;
static const u32 MODE_ANALOG = 0x73;
static const u32 MODE_DS2_NATIVE = 0x79;

enum PadCommands {
    CMD_SET_VREF_PARAM = 0x40,
    CMD_QUERY_DS2_ANALOG_MODE = 0x41,
    CMD_READ_DATA_AND_VIBRATE = 0x42,
    CMD_CONFIG_MODE = 0x43,
    CMD_SET_MODE_AND_LOCK = 0x44,
    CMD_QUERY_MODEL_AND_MODE = 0x45,
    CMD_QUERY_ACT = 0x46,  // ??
    CMD_QUERY_COMB = 0x47, // ??
    CMD_QUERY_MODE = 0x4C, // QUERY_MODE ??
    CMD_VIBRATION_TOGGLE = 0x4D,
    CMD_SET_DS2_NATIVE_MODE = 0x4F // SET_DS2_NATIVE_MODE
};

// The state of the PS2 bus
class QueryInfo
{
    public:
        u8 port;
        u8 slot;
        u8 lastByte;
        u8 currentCommand;
        u8 numBytes;
        u8 queryDone;
        u8 response[42];

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
};

// Freeze data, for a single pad.  Basically has all pad state that a PS2 can set.
struct PadFreezeData
{
    // Digital / Analog / DS2 Native
    u8 mode;

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

class Pad : public PadFreezeData
{
    public:
        // Lilypad store here the state of PC pad

        void rumble(int port);
        void set_vibrate(int motor, u8 val);
        void reset_vibrate();
        void reset();

        void set_mode(int mode);

        static void reset_all();
        static void stop_vibrate_all();
        static void rumble_all();
};

// Full state to manage save state
struct PadPluginFreezeData
{
    char format[8];
    u32 version;
    // active slot for port
    u8 slot[2];
    PadFreezeData padData[2][4];
    QueryInfo query;
};

extern QueryInfo query;
extern std::array<s32, 2> slots;
extern std::array<std::array<Pad, 4>, 2> pads;

extern u8 pad_start_poll(u8 pad);
extern u8 pad_poll(u8 value);