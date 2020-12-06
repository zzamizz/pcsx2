/*  LilyPad - Pad plugin for PS2 Emulator
 *  Copyright (C) 2002-2017  PCSX2 Dev Team/ChickenLiver
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser General Public License as published by the Free
 *  Software Found- ation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with PCSX2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "Devices.h"
#include <vector>

class InputDeviceManager
{
public:
    std::vector<Device*> devices;
    int numDevices;

    void ClearDevices();

    // When refreshing devices, back up old devices, then
    // populate this with new devices, then call copy bindings.
    // All old bindings are copied to matching devices.

    // When old devices are missing, I do a slightly more careful search
    // using productIDs and then (in desperation) displayName.
    // Finally create new dummy devices if no matches found.
    void CopyBindings(int numDevices, std::vector<Device*> devices);


    InputDeviceManager();
    ~InputDeviceManager();

    void AddDevice(Device *d);
    Device *GetActiveDevice(InitInfo *info, unsigned int *uid, int *index, int *value);
    void Update(InitInfo *initInfo);

    // Called after reading state, after Update().
    void PostRead();

    void SetEffect(unsigned char port, unsigned int slot, unsigned char motor, unsigned char force);

    // Update does this as needed.
    // void GetInput(void *v);
    void ReleaseInput();

    void DisableDevice(int index);
    inline void EnableDevice(int i)
    {
        devices[i]->enabled = 1;
    }

    void EnableDevices(DeviceType type, DeviceAPI api);
    void DisableAllDevices();
};

extern InputDeviceManager *dm;

#endif
