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

// Map properly later.
void init_mappings()
{
    m_pad_to_sdl[PAD_L2] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    m_pad_to_sdl[PAD_R2] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
    m_pad_to_sdl[PAD_L1] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    m_pad_to_sdl[PAD_R1] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    m_pad_to_sdl[PAD_TRIANGLE] = SDL_CONTROLLER_BUTTON_Y;
    m_pad_to_sdl[PAD_CIRCLE] = SDL_CONTROLLER_BUTTON_B;
    m_pad_to_sdl[PAD_CROSS] = SDL_CONTROLLER_BUTTON_A;
    m_pad_to_sdl[PAD_SQUARE] = SDL_CONTROLLER_BUTTON_X;
    m_pad_to_sdl[PAD_SELECT] = SDL_CONTROLLER_BUTTON_BACK;
    m_pad_to_sdl[PAD_L3] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
    m_pad_to_sdl[PAD_R3] = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    m_pad_to_sdl[PAD_START] = SDL_CONTROLLER_BUTTON_START;
    m_pad_to_sdl[PAD_UP] = SDL_CONTROLLER_BUTTON_DPAD_UP;
    m_pad_to_sdl[PAD_RIGHT] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    m_pad_to_sdl[PAD_DOWN] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    m_pad_to_sdl[PAD_LEFT] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    m_pad_to_sdl[PAD_L_UP] = SDL_CONTROLLER_AXIS_LEFTY;
    m_pad_to_sdl[PAD_L_RIGHT] = SDL_CONTROLLER_AXIS_LEFTX;
    m_pad_to_sdl[PAD_L_DOWN] = SDL_CONTROLLER_AXIS_LEFTY;
    m_pad_to_sdl[PAD_L_LEFT] = SDL_CONTROLLER_AXIS_LEFTX;
    m_pad_to_sdl[PAD_R_UP] = SDL_CONTROLLER_AXIS_RIGHTY;
    m_pad_to_sdl[PAD_R_RIGHT] = SDL_CONTROLLER_AXIS_RIGHTX;
    m_pad_to_sdl[PAD_R_DOWN] = SDL_CONTROLLER_AXIS_RIGHTY;
    m_pad_to_sdl[PAD_R_LEFT] = SDL_CONTROLLER_AXIS_RIGHTX;
}

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

    init_mappings();
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
        int value = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)m_pad_to_sdl[input]);
        value *= k;
        return (abs(value) > deadzone) ? value : 0;
    }

    // Handle triggers which range from 0 to +32k. They must be converted to 0-255 range
    if (input == PAD_L2 || input == PAD_R2) 
    {
        int value = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)m_pad_to_sdl[input]);
        return (value > deadzone) ? value / 128 : 0;
    }

    // Remain buttons
    value = SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)m_pad_to_sdl[input]);

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

            if (value != 0)
                ps2_gamepad[cpad].press(i, value);
            else
                ps2_gamepad[cpad].release(i);
        }
    }
}