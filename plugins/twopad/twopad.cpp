/*  TwoPAD - author: arcum42(@gmail.com)
 *  Copyright (C) 2019
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
#include "mt_queue.h"
#include "poll.h"
#include "ps2_pad.h"

const u32 version = PS2E_PAD_VERSION;
const u32 revision = 1;
const u32 build = 0; // increase that with each version

const u32 pad_save_state_version ((revision << 8) | (build << 0));

std::string ini_path("inis/");
std::string log_path("logs/");

keyEvent event;
static keyEvent s_event;
MtQueue<keyEvent> g_ev_fifo;

std::array<ps2_pad, 2>ps2_gamepad;

//Linux
#if defined(__unix__)
Display *GSdsp;
Window GSwin;

#include "linux/keyboard_x11.h"
#endif

EXPORT_C_(u32) PS2EgetLibType()
{
    return PS2E_LT_PAD;
}

EXPORT_C_(const char *) PS2EgetLibName()
{
    return "twopad";
}

EXPORT_C_(u32) PS2EgetLibVersion2(u32 type)
{
    return (version << 16) | (revision << 8) | build;
}

// Called once.
EXPORT_C_(s32) PADinit(u32 flags)
{
    Pad::reset_all();
    query.reset();
    ps2_gamepad[0].Init();
    ps2_gamepad[1].Init();
    slots = {0, 0};

    return 0;
}

// Called if emulation is started or restarted.
EXPORT_C_(s32) PADopen(void *pDsp)
{
    g_ev_fifo.reset();

#if defined(__unix__)
    GSdsp = *(Display **)pDsp;
    GSwin = (Window) * (((u32 *)pDsp) + 1);

    SetAutoRepeat(false);
#endif

    return 0;
}

// Called when emulation is stopped for any reason.
EXPORT_C_(void) PADclose()
{
    g_ev_fifo.reset();

#if defined(__unix__)
    SetAutoRepeat(true);
#endif
}

// Called once.
EXPORT_C_(void) PADshutdown()
{
    g_ev_fifo.reset();
}

// Get the path to the ini directory.
EXPORT_C_(void) PADsetSettingsDir(const char *dir)
{
    std::string str_dir(dir);

    if (!str_dir.empty()) ini_path = str_dir;
}

// Get the path to the log directory.
EXPORT_C_(void) PADsetLogDir(const char *dir)
{
    std::string str_dir(dir);

    if (!str_dir.empty()) log_path = str_dir;
}

// PADkeyEvent is called every vsync. (Return NULL if there's no event.)
EXPORT_C_(keyEvent *) PADkeyEvent()
{
    s_event = event;
    event.evt = 0;
    event.key = 0;
    return &s_event;
}

EXPORT_C_(u8) PADstartPoll(int pad)
{
    return pad_start_poll(pad);
}

EXPORT_C_(u8) PADpoll(u8 value)
{
    return pad_poll(value);
}

// 1 for pad 1 support, 2 for pad 2 support, and 3 for both. 
// Everyone returns 3.
EXPORT_C_(u32) PADquery()
{
    return 3;
}

// Call to give a hint to the PAD plugin to query for the keyboard state. A
// good plugin will query the OS for keyboard state ONLY in this function.

// This function is necessary when multithreading because otherwise
// the PAD plugin can get into deadlocks with the thread that really owns
// the window (and input). 

// Note that PADupdate can be called from a different thread than the other functions, 
// so mutex or other multithreading primitives have to be added to maintain data integrity.
EXPORT_C_(void) PADupdate(int pad)
{
#if defined(__unix__)
    // Gamepad inputs don't count as an activity. Therefore screensaver will
    // be fired after a couple of minutes.
    // Emulate an user activity.
    static int count = 0;
    count++;

    // 1 call every 4096 Vsync is enough.
    if ((count & 0xFFF) == 0) 
    {
        XResetScreenSaver(GSdsp);
    }
#endif

    // Actually PADupdate is always called with pad == 0. So you need to update both
    // pads. -- Gregory

    // Poll keyboard/mouse event. There is currently no way to separate pad0 from pad1 event.
    // So we will populate both pads at the same time.
}

// Send a key event from wx-gui to pad
// Note: On linux GSOpen2, wx-gui and pad share the same event buffer. Wx-gui reads and deletes event
// before the pad saw them. So the gui needs to send them back to the pad.
EXPORT_C_(void) PADWriteEvent(keyEvent &evt)
{
    g_ev_fifo.push(evt);
}

EXPORT_C_(void) PADgsDriverInfo(GSdriverInfo *info)
{

}

EXPORT_C_(s32) PADfreeze(u8 mode, freezeData *data)
{
    return 0;
}

// Returns 1 if the pad plugin wants a multitap on the specified port, and 0 otherwise.
EXPORT_C_(s32) PADqueryMtap(u8 port)
{
    port--;
    if (port > 1) return 0;
    //return config.multitap[port];
    return 0;
}

// Sets the active pad slot for the specified port.
// Both numbers are 1-based indices.  We should return 0 if there's no pad on the specified slot.  
// Even if PADqueryMtap(port) returns 0, we should handle this properly for slot != 1, so the
// emulator can allow Multitap to be enabled/disabled elsewhere.
EXPORT_C_(s32) PADsetSlot(u8 port, u8 slot)
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

EXPORT_C_(void) PADconfigure()
{

}

EXPORT_C_(void) PADabout()
{

}

EXPORT_C_(s32) PADtest()
{
    return 0;
}