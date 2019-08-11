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

#include <SDL.h>
#include <SDL_haptic.h>

#include <vector>

#include "twopad.h"
#include "sdl_controller.h"

#include <signal.h> // sigaction

std::vector<sdl_controller*> sdl_pad;

void init_sdl()
{
    uint32_t flag = SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER;

    if ((SDL_WasInit(0) & flag) != flag) 
    {
        // Tell SDL to catch event even if the windows isn't focussed
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

        if (SDL_Init(flag) < 0) return;

        // WTF! Give me back the control of my system.
        struct sigaction action = {};
        action.sa_handler = SIG_DFL;
        sigaction(SIGINT, &action, nullptr);
        sigaction(SIGTERM, &action, nullptr);

        SDL_JoystickEventState(SDL_QUERY);
        SDL_GameControllerEventState(SDL_QUERY);
        SDL_EventState(SDL_CONTROLLERDEVICEADDED, SDL_ENABLE);
        SDL_EventState(SDL_CONTROLLERDEVICEREMOVED, SDL_ENABLE);

        SDL_GameControllerAddMappingsFromRW(SDL_RWFromFile("gamecontrollerdb.txt", "rb"), 1);
    }

    scan_controllers();
}

void scan_controllers()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i) 
    {
        if (SDL_IsGameController(i)) 
        {
            sdl_controller *temp = new sdl_controller;

            temp->controller = SDL_GameControllerOpen(i);
            temp->joystick = SDL_GameControllerGetJoystick(temp->controller);
            temp->name = SDL_GameControllerNameForIndex(i);
            temp->id = i;
            temp->map_defaults(0);
            sdl_pad.push_back(temp);

            printf("Index \'%i\' is a compatible controller, named \'%s\'\n", i, SDL_GameControllerNameForIndex(i));
        } 
        else 
        {
            printf("Index \'%i\' is not a compatible controller.\n", i);
        }
    }
}

int sdl_controller::get_input(gamePadValues input)
{
    float k = sensitivity / 100.0; // convert sensibility to float
    int value = 0;

    // Handle analog inputs which range from -32k to +32k. Range conversion is handled later in the controller
    if (IsAnalogKey(input)) 
    {
        int value = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)key_to_sdl[slot][input]);
        value *= k;
        return (abs(value) > deadzone) ? value : 0;
    }

    // Handle triggers which range from 0 to +32k. They must be converted to 0-255 range
    if (input == PAD_L2 || input == PAD_R2) 
    {
        int value = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)key_to_sdl[slot][input]);
        return (value > deadzone) ? value / 128 : 0;
    }

    // Remain buttons
    value = SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)key_to_sdl[slot][input]);

    return value ? 0xFF : 0; // Max pressure
}

void sdl_events()
{
    SDL_Event sdlEvent;

    while(SDL_PollEvent(&sdlEvent)) 
    {
        switch( sdlEvent.type ) 
        {
            case SDL_CONTROLLERDEVICEADDED:
                //scan_controllers();
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                //scan_controllers();
                break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                //OnControllerButton( sdlEvent.cbutton );
                break;

            case SDL_CONTROLLERAXISMOTION:
                //OnControllerAxis( sdlEvent.caxis );
                break;
        }
    }
}

void PollForJoystickInput()
{
    for (int cpad = 0; cpad < 2; cpad++) 
    {
        for (int i= 0; i < MAX_KEYS; i++) 
        {
            s32 value = 0;
            
            for(auto &con : sdl_pad)
            {
                value = con->get_input((gamePadValues)i);
            }

            ps2_gamepad[cpad].set(i, value);
        }
    }
}
