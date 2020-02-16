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
        fclose(f);
    }
    else
    {
        printf("Unable to save twopad configuration.\n");
        return;
    }
    
}

void load_config()
{
    FILE *f;
    const std::string ini_file(ini_path + "TwoPad.ini");
    u32 value = 0;

    f = fopen(ini_file.c_str(), "r");
    if (f != nullptr)
    {
        for (int pad = 0; pad < 2; pad++)
        {
            for (int key = 0; key < MAX_KEYS; key++)
            {
                char str[256];
                u32 temp = 0;

                sprintf(str, "[%d][%d] = 0x%%x\n", pad, key);

                if (fscanf(f, str, &temp) <= 0)
                    temp = 0;
                else
                    keys->set_key(pad, key, temp);
            }
        }
        
        if (fscanf(f, "auto_repeat = %x\n", &value) > 0)
            keys->auto_repeat = value;

        fclose(f);
    }
    else
    {
        printf("Unable to load twopad configuration.\n");
    }

}