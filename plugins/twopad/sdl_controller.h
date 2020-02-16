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

#include <SDL.h>
#include <SDL_haptic.h>

#include <vector>
#include "twopad.h"

extern void init_sdl();
extern void sdl_events();
extern void scan_controllers();
extern void PollForJoystickInput();

class sdl_controller
{
    public:
        int id = 0;
        u8 slot = 0;
        int deadzone = 1500;
        int sensitivity = 100;

        bool connected = false;
        bool rumble_supported = false;
        
        std::string name;
        std::string guid;

        SDL_GameController* controller;
        SDL_Joystick *joystick;
        SDL_Haptic *haptic;
        SDL_HapticEffect big_motor, small_motor;
        int big_motor_id = -1, small_motor_id = -1;
        Sint16 intensity = 32767;

        std::array<std::array<int, MAX_KEYS>,2> key_to_sdl;

        sdl_controller(int i);
        ~sdl_controller();

        int get_input(int input);
        void map_defaults(u8 pad)
        {
            key_to_sdl[pad][PAD_L2] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
            key_to_sdl[pad][PAD_R2] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
            key_to_sdl[pad][PAD_L1] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
            key_to_sdl[pad][PAD_R1] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
            key_to_sdl[pad][PAD_TRIANGLE] = SDL_CONTROLLER_BUTTON_Y;
            key_to_sdl[pad][PAD_CIRCLE] = SDL_CONTROLLER_BUTTON_B;
            key_to_sdl[pad][PAD_CROSS] = SDL_CONTROLLER_BUTTON_A;
            key_to_sdl[pad][PAD_SQUARE] = SDL_CONTROLLER_BUTTON_X;
            key_to_sdl[pad][PAD_SELECT] = SDL_CONTROLLER_BUTTON_BACK;
            key_to_sdl[pad][PAD_L3] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
            key_to_sdl[pad][PAD_R3] = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
            key_to_sdl[pad][PAD_START] = SDL_CONTROLLER_BUTTON_START;
            key_to_sdl[pad][PAD_UP] = SDL_CONTROLLER_BUTTON_DPAD_UP;
            key_to_sdl[pad][PAD_RIGHT] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
            key_to_sdl[pad][PAD_DOWN] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
            key_to_sdl[pad][PAD_LEFT] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
            key_to_sdl[pad][PAD_L_UP] = SDL_CONTROLLER_AXIS_LEFTY;
            key_to_sdl[pad][PAD_L_RIGHT] = SDL_CONTROLLER_AXIS_LEFTX;
            key_to_sdl[pad][PAD_L_DOWN] = SDL_CONTROLLER_AXIS_LEFTY;
            key_to_sdl[pad][PAD_L_LEFT] = SDL_CONTROLLER_AXIS_LEFTX;
            key_to_sdl[pad][PAD_R_UP] = SDL_CONTROLLER_AXIS_RIGHTY;
            key_to_sdl[pad][PAD_R_RIGHT] = SDL_CONTROLLER_AXIS_RIGHTX;
            key_to_sdl[pad][PAD_R_DOWN] = SDL_CONTROLLER_AXIS_RIGHTY;
            key_to_sdl[pad][PAD_R_LEFT] = SDL_CONTROLLER_AXIS_RIGHTX;
        }
    void upload_haptic_effects();
    bool set_triangle_effect(SDL_HapticEffect &effect, int &id);
    bool set_sine_effect(SDL_HapticEffect &effect, int &id);
    void vibrate(int type, int cpad);
};

extern std::vector<sdl_controller*> sdl_pad;