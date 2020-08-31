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

#include "keyboard_x11.h"

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

void AnalyzeKeyEvent(keyEvent &evt)
{
    KeySym key = (KeySym)evt.key;
    int pad = 0;
    int index = -1;

    // Check to see if there is a key with value key that is set to any button on any controller.
    // If there is, pad is the pad it is on, and index is the button value.

    // Needs to be written.
    
    /*for (int cpad = 0; cpad < 2; cpad++)
    {
        int tmp_index = get_keyboard_key(cpad, key);
        if (tmp_index != -1)
        {
            pad = cpad;
            index = tmp_index;
        }
    }*/

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
                ps2_gamepad[pad].press(index);

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