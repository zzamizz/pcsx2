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

#pragma once

#include <SDL.h>
#include <SDL_haptic.h>

#include <vector>
#include "twopad.h"
#define NB_EFFECT 2 // Don't use more than two, ps2 only has one for big motor and one for small(like most systems).

extern void init_sdl();
extern void sdl_events();
extern void scan_controllers();
extern void PollForJoystickInput();

static std::array<int, MAX_KEYS> m_pad_to_sdl;

class sdl_controller
{
    public:
        int id = 0;
        int deadzone = 1500;
        int sensitivity = 100;

        bool connected = false;
        bool haptic = false;
        
        std::string name;
        std::string guid;

        SDL_GameController* controller;
        SDL_Joystick *joystick;

        int get_input(gamePadValues input);
};
