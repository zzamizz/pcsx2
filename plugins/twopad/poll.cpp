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

#include "twopad.h"
#include "poll.h"
#include <algorithm>
#include <array>

QueryInfo query;

std::array<s32, 2> slots = {0,0};
std::array<std::array<Pad, 4>, 2> pads;


//////////////////////////////////////////////////////////////////////
// QueryInfo implementation
//////////////////////////////////////////////////////////////////////

void QueryInfo::reset()
{
    port = 0;
    slot = 0;
    lastByte = 1;
    currentCommand = 0;
    numBytes = 0;
    queryDone = 1;
    std::fill_n(response, sizeof(response), 0xF3);
}

u8 QueryInfo::start_poll(int thePort)
{
    if (port > 1)
    {
        reset();
        return 0;
    }

    queryDone = 0;
    port = thePort;
    slot = slots[port];
    numBytes = 2;
    lastByte = 0;

    return 0xFF;
}

//////////////////////////////////////////////////////////////////////
// Pad implementation
//////////////////////////////////////////////////////////////////////

void Pad::set_mode(int theMode)
{
    mode = theMode;
}

void Pad::set_vibrate(int motor, u8 val)
{
    nextVibrate[motor] = val;
}

void Pad::reset_vibrate()
{
    set_vibrate(0, 0);
    set_vibrate(1, 0);
    memset(vibrate, 0xFF, sizeof(vibrate));
    vibrate[0] = 0x5A;
}

void Pad::reset()
{
    memset(this, 0, sizeof(PadFreezeData));

    set_mode(MODE_DIGITAL);
    umask[0] = umask[1] = 0xFF;

    // Sets up vibrate variable.
    reset_vibrate();
}

void Pad::rumble(int port)
{
    for (int motor = 0; motor < 2; motor++)
    {
        // TODO:  Probably be better to send all of these at once.
        if (nextVibrate[motor] | currentVibrate[motor])
        {
            currentVibrate[motor] = nextVibrate[motor];

            if (ps2_gamepad[port].real != nullptr) ps2_gamepad[port].real->vibrate(motor, port);
        }
    }
}

void Pad::stop_vibrate_all()
{
    for (std::array<Pad, 4>& gamepad : pads)
    {
        gamepad[0].reset_vibrate();
        gamepad[1].reset_vibrate();
    }
}

void Pad::reset_all()
{
    for (std::array<Pad, 4>& gamepad : pads)
    {
        gamepad[0].reset();
        gamepad[1].reset();
    }
}

void Pad::rumble_all()
{
    for (std::array<Pad, 4>& gamepad : pads)
    {
        gamepad[0].rumble(0);
        gamepad[1].rumble(1);
    }
}


//////////////////////////////////////////////////////////////////////
// Pad implementation
//////////////////////////////////////////////////////////////////////

inline bool IsDualshock2()
{
    return true;
}

u8 pad_start_poll(u8 pad)
{
    return query.start_poll(pad - 1);
}

u8 pad_poll(u8 value)
{
    if (query.lastByte + 1 >= query.numBytes)
    {
        return 0;
    }

    if (query.lastByte && query.queryDone)
    {
        return query.response[++query.lastByte];
    }

    Pad *pad = &pads[query.port][query.slot];

    if (query.lastByte == 0)
    {
        query.lastByte++;
        query.currentCommand = value;

        switch (value)
        {
            case CMD_CONFIG_MODE:
                if (pad->config)
                {
                    // In config mode.  Might not actually be leaving it.
                    query.set_result(configExit);
                    return 0xF3;
                }
                //break; // Comment out if it breaks anything.

            case CMD_READ_DATA_AND_VIBRATE:
            {
                query.response[2] = 0x5A;

                u16 buttons = ps2_gamepad[query.port].get();

                query.numBytes = 5;

                query.response[3] = (buttons >> 8) & 0xFF;
                query.response[4] = (buttons >> 0) & 0xFF;

                if (pad->mode != MODE_DIGITAL)
                {
                    query.numBytes = 9;

                    query.response[5] = ps2_gamepad[query.port].get(PAD_R_RIGHT);
                    query.response[6] = ps2_gamepad[query.port].get(PAD_R_UP);
                    query.response[7] = ps2_gamepad[query.port].get(PAD_L_RIGHT);
                    query.response[8] = ps2_gamepad[query.port].get(PAD_L_UP);

                    if (pad->mode != MODE_ANALOG)
                    {
                        query.numBytes = 21;

                        query.response[9] = ps2_gamepad[query.port].get_result(buttons, PAD_RIGHT);
                        query.response[10] = ps2_gamepad[query.port].get_result(buttons, PAD_LEFT);
                        query.response[11] = ps2_gamepad[query.port].get_result(buttons, PAD_UP);
                        query.response[12] = ps2_gamepad[query.port].get_result(buttons, PAD_DOWN);

                        query.response[13] = ps2_gamepad[query.port].get_result(buttons, PAD_TRIANGLE);
                        query.response[14] = ps2_gamepad[query.port].get_result(buttons, PAD_CIRCLE);
                        query.response[15] = ps2_gamepad[query.port].get_result(buttons, PAD_CROSS);
                        query.response[16] = ps2_gamepad[query.port].get_result(buttons, PAD_SQUARE);

                        query.response[17] = ps2_gamepad[query.port].get_result(buttons, PAD_L1);
                        query.response[18] = ps2_gamepad[query.port].get_result(buttons, PAD_R1);
                        query.response[19] = ps2_gamepad[query.port].get_result(buttons, PAD_L2);
                        query.response[20] = ps2_gamepad[query.port].get_result(buttons, PAD_R2);
                    }
                }
            }

                query.lastByte = 1;
                return pad->mode;

            case CMD_SET_VREF_PARAM:
                query.set_final_result(noclue);
                break;

            case CMD_QUERY_DS2_ANALOG_MODE:
                // Right?  Wrong?  No clue.
                if (pad->mode == MODE_DIGITAL)
                {
                    queryMaskMode[1] = queryMaskMode[2] = queryMaskMode[3] = 0;
                    queryMaskMode[6] = 0x00;
                }
                else
                {
                    queryMaskMode[1] = pad->umask[0];
                    queryMaskMode[2] = pad->umask[1];
                    queryMaskMode[3] = 0x03;
                    queryMaskMode[6] = 0x5A;
                }
                query.set_final_result(queryMaskMode);
                break;

            case CMD_SET_MODE_AND_LOCK:
                query.set_result(setMode);
                pad->reset_vibrate();
                break;

            case CMD_QUERY_MODEL_AND_MODE:
                query.set_final_result(IsDualshock2() ? queryModelDS2 : queryModelDS1);
                
                // Not digital mode.
                query.response[5] = (pad->mode & 0xF) != 1;
                break;

            case CMD_QUERY_ACT:
                query.set_result(queryAct[0]);
                break;

            case CMD_QUERY_COMB:
                query.set_final_result(queryComb);
                break;

            case CMD_QUERY_MODE:
                query.set_result(queryMode);
                break;

            case CMD_VIBRATION_TOGGLE:
                memcpy(query.response + 2, pad->vibrate, 7);
                query.numBytes = 9;
                pad->reset_vibrate();
                break;

            case CMD_SET_DS2_NATIVE_MODE:
                if (IsDualshock2())
                {
                    query.set_result(setNativeMode);
                }
                else
                {
                    query.set_final_result(setNativeMode);
                }
                break;

            default:
                query.numBytes = 0;
                query.queryDone = 1;
                break;
        }

        return 0xF3;
    }
    else
    {
        query.lastByte++;

        switch (query.currentCommand)
        {
            case CMD_READ_DATA_AND_VIBRATE:
                if (query.lastByte == pad->vibrateI[0])
                    pad->set_vibrate(1, 255 * (value & 1));
                else if (query.lastByte == pad->vibrateI[1])
                    pad->set_vibrate(0, value);
                break;

            case CMD_CONFIG_MODE:
                if (query.lastByte == 3)
                {
                    query.queryDone = 1;
                    pad->config = value;
                }
                break;

            case CMD_SET_MODE_AND_LOCK:
                if (query.lastByte == 3 && value < 2)
                {
                    pad->set_mode(value ? MODE_ANALOG : MODE_DIGITAL);
                }
                else if (query.lastByte == 4)
                {
                    if (value == 3)
                        pad->modeLock = 3;
                    else
                        pad->modeLock = 0;

                    query.queryDone = 1;
                }
                break;

            case CMD_QUERY_ACT:
                if (query.lastByte == 3)
                {
                    if (value < 2) query.set_result(queryAct[value]);
                    query.queryDone = 1;
                }
                break;

            case CMD_QUERY_MODE:
                if (query.lastByte == 3 && value < 2)
                {
                    query.response[6] = 4 + value * 3;
                    query.queryDone = 1;
                }
                break;

            case CMD_VIBRATION_TOGGLE:
                if (query.lastByte >= 3)
                {
                    if (value == 0)
                    {
                        pad->vibrateI[0] = (u8)query.lastByte;
                    }
                    else if (value == 1)
                    {
                        pad->vibrateI[1] = (u8)query.lastByte;
                    }
                    pad->vibrate[query.lastByte - 2] = value;
                }
                break;

            case CMD_SET_DS2_NATIVE_MODE:
                if (query.lastByte == 3 || query.lastByte == 4)
                {
                    pad->umask[query.lastByte - 3] = value;
                }
                else if (query.lastByte == 5)
                {
                    if (!(value & 1))
                    {
                        pad->set_mode(MODE_DIGITAL);
                    }
                    else if (!(value & 2))
                    {
                        pad->set_mode(MODE_ANALOG);
                    }
                    else
                    {
                        pad->set_mode(MODE_DS2_NATIVE);
                    }
                }
                break;

            default:
                return 0;
        }

        return query.response[query.lastByte];
    }
}