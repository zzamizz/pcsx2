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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <array>
#include <map>

#include "keyboard_x11.h"
#include "ps2_pad.h"

std::map<int, Double> key_to_x11_map;

extern Display *GSdsp;
extern Window GSwin;

void SetAutoRepeat(bool autorep)
{
    //if (toggleAutoRepeat)
    {
        if (autorep)
            XAutoRepeatOn(GSdsp);
        else
            XAutoRepeatOff(GSdsp);
    }
}

static bool s_grab_input = false;
static bool s_Shift = false;

static unsigned int s_previous_mouse_x = 0;
static unsigned int s_previous_mouse_y = 0;

void init_x11_keys()
{
    key_to_x11_map[XK_a] = { 0, PAD_L2 };
    key_to_x11_map[XK_semicolon] = { 0, PAD_R2 };
    key_to_x11_map[XK_w] = { 0, PAD_L1 };
    key_to_x11_map[XK_p] = { 0, PAD_R1 };
    key_to_x11_map[XK_q] = { 0, PAD_L3 };
    key_to_x11_map[XK_o] = { 0, PAD_R3 };

    key_to_x11_map[XK_i] = { 0, PAD_TRIANGLE };
    key_to_x11_map[XK_l] = { 0, PAD_CIRCLE };
    key_to_x11_map[XK_k] = { 0, PAD_CROSS };
    key_to_x11_map[XK_j] = { 0, PAD_SQUARE };

    key_to_x11_map[XK_v] = { 0, PAD_SELECT };
    key_to_x11_map[XK_n] = { 0, PAD_START };

    key_to_x11_map[XK_e] = { 0, PAD_UP };
    key_to_x11_map[XK_f] = { 0, PAD_RIGHT };
    key_to_x11_map[XK_d] = { 0, PAD_DOWN };
    key_to_x11_map[XK_s] = { 0, PAD_LEFT };

    key_to_x11_map[XK_Up] = { 0, PAD_L_UP };
    key_to_x11_map[XK_Right] = { 0, PAD_L_RIGHT };
    key_to_x11_map[XK_Down] = { 0, PAD_L_DOWN };
    key_to_x11_map[XK_Left] = { 0, PAD_L_LEFT };

    key_to_x11_map[XK_KP_8] = { 0, PAD_R_UP };
    key_to_x11_map[XK_KP_6] = { 0, PAD_R_RIGHT };
    key_to_x11_map[XK_KP_2] = { 0, PAD_R_DOWN };
    key_to_x11_map[XK_KP_4] = { 0, PAD_R_LEFT };
}

void AnalyzeKeyEvent(keyEvent &evt)
{
    KeySym key = (KeySym)evt.key;
    int pad = 0;
    int index = -1;

    // Check to see if there is a key with value key that is set to any button on any controller.
    // If there is, pad is the pad it is on, and index is the button value.
    if (key_to_x11_map.count(evt.key) > 0) 
    {
        pad = key_to_x11_map[evt.key][0];
        index = key_to_x11_map[evt.key][1];
    }
    //if (index != -1) printf("Key pressed: key = %i\n", index);

    switch (evt.evt)
    {
        case KeyPress:
            // Shift F12 is not yet use by pcsx2. So keep it to grab/ungrab input
            // I found it very handy vs the automatic fullscreen detection
            // 1/ Does not need to detect full-screen
            // 2/ Can use a debugger in full-screen
            // 3/ Can grab input in window without the need of a pixelated full-screen
            if (key == XK_Shift_R || key == XK_Shift_L) s_Shift = true;

            if (key == XK_F12 && s_Shift)
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
                        ps2_gamepad[pad].press(index, -MAX_ANALOG_VALUE);
                        break;

                    case PAD_R_RIGHT:
                    case PAD_R_DOWN:
                    case PAD_L_RIGHT:
                    case PAD_L_DOWN:
                        ps2_gamepad[pad].press(index, MAX_ANALOG_VALUE);
                        break;
                }
            }
            else if (index != -1)
            {
                ps2_gamepad[pad].press(index);
            }

            event.evt = KEYPRESS;
            event.key = key;
            break;

        case KeyRelease:
            if (key == XK_Shift_R || key == XK_Shift_L)  s_Shift = false;

            if (index != -1) ps2_gamepad[pad].release(index);

            event.evt = KEYRELEASE;
            event.key = key;
            break;

        case FocusIn:
            break;

        case FocusOut:
            s_Shift = false;
            break;

        case ButtonPress:
            if (index != -1) ps2_gamepad[pad].press(pad, index);
            break;

        case ButtonRelease:
            if (index != -1) ps2_gamepad[pad].release(index);
            break;

        case MotionNotify:
            // FIXME: How to handle when the mouse does not move, no event generated!!!
            // 1/ small move == no move. Cons : can not do small movement
            // 2/ use a watchdog timer thread
            // 3/ ??? idea welcome ;)
            /*if (conf->pad_options[pad].mouse_l | conf->pad_options[pad].mouse_r)
            {
                u32 pad_x, pad_y;

                // Note when both PADOPTION_MOUSE_R and PADOPTION_MOUSE_L are set, take only the right one
                if (conf->pad_options[pad].mouse_r)
                {
                    pad_x = PAD_R_RIGHT;
                    pad_y = PAD_R_UP;
                }
                else
                {
                    pad_x = PAD_L_RIGHT;
                    pad_y = PAD_L_UP;
                }

                u32 x = evt.key & 0xFFFF;
                u32 value = (s_previous_mouse_x > x) ? s_previous_mouse_x - x : x - s_previous_mouse_x;
                //value *= conf->get_sensibility();

                if (x == 0)
                    ps2_gamepad[pad].press(pad_x, -MAX_ANALOG_VALUE);
                else if (x == 0xFFFF)
                    ps2_gamepad[pad].press(pad_x, MAX_ANALOG_VALUE);
                else if (x < (s_previous_mouse_x - 2))
                    ps2_gamepad[pad].press(pad_x, -value);
                else if (x > (s_previous_mouse_x + 2))
                    ps2_gamepad[pad].press(pad_x, value);
                else
                    ps2_gamepad[pad].release(pad_x);


                u32 y = evt.key >> 16;
                value = (s_previous_mouse_y > y) ? s_previous_mouse_y - y : y - s_previous_mouse_y;
                //value *= conf->get_sensibility();

                if (y == 0)
                    ps2_gamepad[pad].press(pad_y, -MAX_ANALOG_VALUE);
                else if (y == 0xFFFF)
                    ps2_gamepad[pad].press(pad_y, MAX_ANALOG_VALUE);
                else if (y < (s_previous_mouse_y - 2))
                    ps2_gamepad[pad].press(pad_y, -value);
                else if (y > (s_previous_mouse_y + 2))
                    ps2_gamepad[pad].press(pad_y, value);
                else
                    ps2_gamepad[pad].release(pad_y);

                s_previous_mouse_x = x;
                s_previous_mouse_y = y;
            }*/
            break;
    }
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