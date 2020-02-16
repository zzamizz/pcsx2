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

#include <iostream>
#include <fstream>
#include <string>

#include "settings.h"
#include "twopad.h"
#include "ps2_pad.h"

#if defined(__unix__)
#include "linux/keyboard_x11.h"
#endif

std::string ini_path("inis/");
std::string log_path("logs/");

// The joy of implementing the same thing over and over.
// Could use wxWidgets to parse it, but I might as well stick to plain C for the moment.
void save_config()
{
    FILE *f;
    const std::string ini_file(ini_path + "TwoPad.ini");

    f = fopen(ini_file.c_str(), "w");
    if (f != nullptr)
    {
        for (int pad = 0; pad < 2; pad++)
        {
            for (int key = 0; key < MAX_KEYS; key++)
            {
                fprintf(f, "[%d][%d] = 0x%x\n", pad, key, keys->get_key(pad, key));
            }
        }
        fprintf(f, "auto_repeat = %x\n", keys->auto_repeat);

        fprintf(f, "rumble[0] = %x\n", ps2_gamepad[0]->rumble);
        fprintf(f, "reversed_lx[0] = %x\n", ps2_gamepad[0]->reversed_lx);
        fprintf(f, "reversed_ly[0] = %x\n", ps2_gamepad[0]->reversed_ly);
        fprintf(f, "reversed_rx[0] = %x\n", ps2_gamepad[0]->reversed_rx);
        fprintf(f, "reversed_ry[0] = %x\n", ps2_gamepad[0]->reversed_ry);

        fprintf(f, "rumble[1] = %x\n", ps2_gamepad[1]->rumble);
        fprintf(f, "reversed_lx[1] = %x\n", ps2_gamepad[1]->reversed_lx);
        fprintf(f, "reversed_ly[1] = %x\n", ps2_gamepad[1]->reversed_ly);
        fprintf(f, "reversed_rx[1] = %x\n", ps2_gamepad[1]->reversed_rx);
        fprintf(f, "reversed_ry[1] = %x\n", ps2_gamepad[1]->reversed_ry);

        fclose(f);
    }
    else
    {
        printf("Unable to save twopad configuration.\n");
        return;
    }
    
}

void init_config()
{
    keys->auto_repeat = true;

    ps2_gamepad[0]->rumble = true;
    ps2_gamepad[0]->reversed_lx = false;
    ps2_gamepad[0]->reversed_ly = false;
    ps2_gamepad[0]->reversed_rx = false;
    ps2_gamepad[0]->reversed_ry = false;

    ps2_gamepad[1]->rumble = true;
    ps2_gamepad[1]->reversed_lx = false;
    ps2_gamepad[1]->reversed_ly = false;
    ps2_gamepad[1]->reversed_rx = false;
    ps2_gamepad[0]->reversed_ry = false;

}

void load_config()
{
    FILE *f;
    const std::string ini_file(ini_path + "TwoPad.ini");
    u32 value = 0;

    init_config();
    keys->init_keys();

    f = fopen(ini_file.c_str(), "r");
    if (f != nullptr)
    {
        for (int pad = 0; pad < 2; pad++)
        {
            for (u32 key = 0; key < MAX_KEYS; key++)
            {
                char str[256];

                sprintf(str, "[%d][%d] = 0x%%x\n", pad, key);

                if (fscanf(f, str, &value) > 0) keys->set_key(pad, key, value);
            }
        }
        
        if (fscanf(f, "auto_repeat = %x\n", &value) > 0) keys->auto_repeat = value;

        if (fscanf(f, "rumble[0] = %x\n", &value) > 0) ps2_gamepad[0]->rumble = value;
        if (fscanf(f, "reversed_lx[0] = %x\n", &value) > 0) ps2_gamepad[0]->reversed_lx = value;
        if (fscanf(f, "reversed_ly[0] = %x\n", &value) > 0) ps2_gamepad[0]->reversed_ly = value;
        if (fscanf(f, "reversed_rx[0] = %x\n", &value) > 0) ps2_gamepad[0]->reversed_rx = value;
        if (fscanf(f, "reversed_ry[0] = %x\n", &value) > 0) ps2_gamepad[0]->reversed_ry = value;

        if (fscanf(f, "rumble[1] = %x\n", &value) > 0) ps2_gamepad[1]->rumble = value;
        if (fscanf(f, "reversed_lx[1] = %x\n", &value) > 0) ps2_gamepad[1]->reversed_lx = value;
        if (fscanf(f, "reversed_ly[1] = %x\n", &value) > 0) ps2_gamepad[1]->reversed_ly = value;
        if (fscanf(f, "reversed_rx[1] = %x\n", &value) > 0) ps2_gamepad[1]->reversed_rx = value;
        if (fscanf(f, "reversed_ry[1] = %x\n", &value) > 0) ps2_gamepad[1]->reversed_ry = value;

        fclose(f);
    }
    else
    {
        printf("Unable to load twopad configuration.\n");
    }
}