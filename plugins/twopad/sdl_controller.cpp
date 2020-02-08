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
#include "ps2_pad.h"
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

    // Assume that if we don't know about a controller on gamepad 1, we want to use the first one in the list.
    if (!ps2_gamepad[0].controller_attached && (ps2_gamepad[0].real == nullptr) && (sdl_pad.size() > 0))
    {
        ps2_gamepad[0].controller_attached = true;
        ps2_gamepad[0].real = sdl_pad.front();
    }
}

sdl_controller::sdl_controller(int i)
{
    id = i;
    controller = SDL_GameControllerOpen(i);
    joystick = SDL_GameControllerGetJoystick(controller);
    name = SDL_GameControllerNameForIndex(i);

    rumble_supported = SDL_JoystickIsHaptic(joystick);
    if (rumble_supported)
    {
        haptic = SDL_HapticOpenFromJoystick(joystick);
        if (haptic == nullptr)
        {
            rumble_supported = false;
        }
        else
        {
            rumble_supported = true;
            rumble = true;
            upload_haptic_effects();
        }
    }

    map_defaults(0);
}

sdl_controller::~sdl_controller()
{
    if (haptic != nullptr) SDL_HapticClose(haptic);
    if (joystick != nullptr) SDL_JoystickClose(joystick);
    if (controller != nullptr) SDL_GameControllerClose(controller);
}

void scan_controllers()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i) 
    {
        if (SDL_IsGameController(i)) 
        {
            sdl_controller *temp = new sdl_controller(i);
            if (temp != nullptr) sdl_pad.push_back(temp);

            printf("Index \'%i\' is a compatible controller, named \'%s\'\n", i, SDL_GameControllerNameForIndex(i));
        } 
        else 
        {
            printf("Index \'%i\' is not a compatible controller.\n", i);
        }
    }
}

bool sdl_controller::set_triangle_effect(SDL_HapticEffect &effect, int &id)
{
    effect.type = SDL_HAPTIC_TRIANGLE;

    SDL_HapticDirection direction;
    direction.type = SDL_HAPTIC_POLAR; // We'll be using polar direction encoding.
    direction.dir[0] = 18000;

    effect.periodic.direction = direction;
    effect.periodic.period = 10;
    effect.periodic.magnitude = intensity; // Effect at maximum instensity
    effect.periodic.offset = 0;
    effect.periodic.phase = 18000;
    effect.periodic.length = 125; // 125ms feels quite near to original
    effect.periodic.delay = 0;
    effect.periodic.attack_length = 0;

    id = SDL_HapticNewEffect(haptic, &effect);
    if (id < 0) return false;
    return true;
}

bool sdl_controller::set_sine_effect(SDL_HapticEffect &effect, int &id)
{
    effect.type = SDL_HAPTIC_SINE;

    SDL_HapticDirection direction;
    direction.type = SDL_HAPTIC_POLAR; // We'll be using polar direction encoding.
    direction.dir[0] = 18000;

    effect.periodic.direction = direction;
    effect.periodic.period = 10;
    effect.periodic.magnitude = intensity; // Effect at maximum instensity
    effect.periodic.offset = 0;
    effect.periodic.phase = 18000;
    effect.periodic.length = 125; // 125ms feels quite near to original
    effect.periodic.delay = 0;
    effect.periodic.attack_length = 0;

    id = SDL_HapticNewEffect(haptic, &effect);
    if (id < 0) return false;
    return true;
}

void sdl_controller::upload_haptic_effects()
{
    uint32_t support = SDL_HapticQuery(haptic);

    if (support & SDL_HAPTIC_TRIANGLE)
    {
        set_triangle_effect(big_motor, big_motor_id);
    }

    if (support & SDL_HAPTIC_SINE)
    {
        set_sine_effect(small_motor, small_motor_id);
    }

    if ((small_motor_id < 0) || (big_motor_id < 0))
    {
        rumble_supported = false;
        rumble = false;
    }
    else
    {
        rumble_supported = true;
        rumble = true;
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

void sdl_controller::vibrate(int type, int cpad)
{
    if ((ps2_gamepad[cpad].controller_attached) && (ps2_gamepad[cpad].real != nullptr))
    {
        sdl_controller *ctl = ps2_gamepad[cpad].real;
        if ((ctl->rumble_supported) && (ctl->rumble) && (ctl->haptic != nullptr))
        {
            SDL_HapticRunEffect(ctl->haptic, type, 1);
        }
    }
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
    for (u32 cpad = 0; cpad < 2; cpad++)
    {
        for (u32 i= 0; i < MAX_KEYS; i++)
        {
            s32 value = 0;

            if (ps2_gamepad[cpad].controller_attached)
            {
                value = ps2_gamepad[cpad].real->get_input((gamePadValues)i);
            }

            ps2_gamepad[cpad].set(i, value);
        }
    }
}
