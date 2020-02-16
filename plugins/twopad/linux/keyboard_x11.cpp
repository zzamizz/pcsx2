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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <array>
#include <map>
#include "mt_queue.h"

#include "keyboard_x11.h"
#include "ps2_pad.h"

//Linux
#if defined(__unix__)
Display *GSdsp;
Window GSwin;
#endif

std::array<x11_map, 2> x11_key_map;

extern Display *GSdsp;
extern Window GSwin;

extern keyEvent event;
extern MtQueue<keyEvent> g_ev_fifo;

static bool s_grab_input = false;
static bool s_Shift = false;

static unsigned int s_previous_mouse_x = 0;
static unsigned int s_previous_mouse_y = 0;

void SetAutoRepeat(bool autorep)
{
    if (GSdsp != nullptr)
    {
        if (autorep)
            XAutoRepeatOn(GSdsp);
        else
            XAutoRepeatOff(GSdsp);
    }
}

void init_x11_keys()
{
    x11_key_map[0][PAD_L2] = XK_a;
    x11_key_map[0][PAD_R2] = XK_semicolon;
    x11_key_map[0][PAD_L1] = XK_w;
    x11_key_map[0][PAD_R1] = XK_p;
    x11_key_map[0][PAD_L3] = XK_q;
    x11_key_map[0][PAD_R3] = XK_o;

    x11_key_map[0][PAD_TRIANGLE] = XK_i;
    x11_key_map[0][PAD_CIRCLE] = XK_l;
    x11_key_map[0][PAD_CROSS] = XK_k;
    x11_key_map[0][PAD_SQUARE] = XK_j;

    x11_key_map[0][PAD_SELECT] = XK_v;
    x11_key_map[0][PAD_START] = XK_n;

    x11_key_map[0][PAD_UP] = XK_e;
    x11_key_map[0][PAD_RIGHT] = XK_f;
    x11_key_map[0][PAD_DOWN] = XK_d;
    x11_key_map[0][PAD_LEFT] = XK_s;

    x11_key_map[0][PAD_L_UP] = XK_Up;
    x11_key_map[0][PAD_L_RIGHT] = XK_Right;
    x11_key_map[0][PAD_L_DOWN] = XK_Down;
    x11_key_map[0][PAD_L_LEFT] = XK_Left;

    x11_key_map[0][PAD_R_UP] = XK_KP_8;
    x11_key_map[0][PAD_R_RIGHT] = XK_KP_6;
    x11_key_map[0][PAD_R_DOWN] = XK_KP_2;
    x11_key_map[0][PAD_R_LEFT] = XK_KP_4;
}

void HandleEvent(keyEvent &evt, int pad, int index)
{
    switch (evt.evt)
    {
        case KeyPress:
            // Shift F12 is not yet used by pcsx2. So keep it to grab/ungrab input
            // I found it very handy vs the automatic fullscreen detection
            // 1/ Does not need to detect full-screen
            // 2/ Can use a debugger in full-screen
            // 3/ Can grab input in window without the need of a pixelated full-screen

            if (evt.key == XK_Shift_R || evt.key == XK_Shift_L) s_Shift = true;

            if (evt.key == XK_F12 && s_Shift)
            {
                if (!s_grab_input)
                {
                    s_grab_input = true;
                    XGrabPointer(GSdsp, GSwin, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, GSwin, None, CurrentTime);
                    XGrabKeyboard(GSdsp, GSwin, True, GrabModeAsync, GrabModeAsync, CurrentTime);
                }
                else
                {
                    s_grab_input = false;
                    XUngrabPointer(GSdsp, CurrentTime);
                    XUngrabKeyboard(GSdsp, CurrentTime);
                }
            }

            // Analog controls.
            if (IsAnalogKey(index))
            {
                switch (index)
                {
                    case PAD_R_LEFT:
                    case PAD_R_UP:
                    case PAD_L_LEFT:
                    case PAD_L_UP:
                        ps2_gamepad[pad]->press(index, -MAX_ANALOG_VALUE);
                        break;

                    case PAD_R_RIGHT:
                    case PAD_R_DOWN:
                    case PAD_L_RIGHT:
                    case PAD_L_DOWN:
                        ps2_gamepad[pad]->press(index, MAX_ANALOG_VALUE);
                        break;
                }
            }
            else if (index != -1)
            {
                ps2_gamepad[pad]->press(index);
            }

            event.evt = KEYPRESS;
            event.key = evt.key;
            break;

        case KeyRelease:
            if (evt.key == XK_Shift_R || evt.key == XK_Shift_L)  s_Shift = false;

            if (index != -1) ps2_gamepad[pad]->release(index);

            event.evt = KEYRELEASE;
            event.key = evt.key;
            break;

        case FocusIn:
            break;

        case FocusOut:
            s_Shift = false;
            break;

        case ButtonPress:
            if (index != -1) ps2_gamepad[pad]->press(pad, index);
            break;

        case ButtonRelease:
            if (index != -1) ps2_gamepad[pad]->release(index);
            break;

        case MotionNotify:
            // Reimplement later.
            break;
    }
}

void AnalyzeKeyEvent(keyEvent &evt)
{
    int pad = 0;
    int index = -1;

    // Check to see if there is a key with value key that is set to any button on any controller.
    // If there is, pad is the pad it is on, and index is the button value.
    for(int cpad = 0; cpad < 2; cpad++)
    {
        for(int button = 0; button < MAX_KEYS; button++)
        {
            if (x11_key_map[cpad][button] == evt.key)
            {
                pad = cpad;
                index = button;
            }
        }
    }
    //if (index != -1) printf("Key pressed: key = %i\n", index);
    HandleEvent(evt, pad, index);
}

void PollForX11KeyboardInput()
{
    keyEvent evt = {0};
    XEvent E = {0};

    // Keyboard input send by PCSX2
    g_ev_fifo.consume_all(AnalyzeKeyEvent);

    // keyboard input
    while (XPending(GSdsp) > 0) 
    {
        XNextEvent(GSdsp, &E);

        // Change the format of the structure to be compatible with GSOpen2
        // mode (events come from pcsx2 not X).
        evt.evt = E.type;
        switch (evt.evt) 
        {
            case MotionNotify:
                evt.key = (E.xbutton.x & 0xFFFF) | (E.xbutton.y << 16);
                break;

            case ButtonRelease:
            case ButtonPress:
                evt.key = E.xbutton.button;
                break;

            default:
                evt.key = (int)XLookupKeysym(&E.xkey, 0);
                break;
        }

        AnalyzeKeyEvent(evt);
    }
}

bool PollX11KeyboardMouseEvent(u32 &pkey)
{
    GdkEvent *ev = gdk_event_get();

    if (ev == nullptr) return false;

    switch(ev->type)
    {
        case GDK_KEY_PRESS:
            pkey = ev->key.keyval != GDK_KEY_Escape ? ev->key.keyval : 0;
            return true;
            break;

        case GDK_BUTTON_PRESS:
            pkey = ev->button.button;
            return true;
            break;
        
        default:
            return false;
            break;
    }

    return false;
}

const char* KeyToString(KeySym k)
{
    return XKeysymToString(k);
}