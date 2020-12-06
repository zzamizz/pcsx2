/*  LilyPad - Pad plugin for PS2 Emulator
 *  Copyright (C) 2002-2014  PCSX2 Dev Team/ChickenLiver
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


#include "Global.h"
#include "Devices.h"
#include "KeyboardQueue.h"
#include "Config.h"

Device::Device(DeviceAPI api, DeviceType d, const wchar_t *displayName, const wchar_t *instanceID, const wchar_t *productID)
{
    memset(pads, 0, sizeof(pads));
    this->api = api;
    type = d;
    this->displayName = wcsdup(displayName);
    if (instanceID)
        this->instanceID = wcsdup(instanceID);
    else
        this->instanceID = wcsdup(displayName);
    this->productID = 0;
    if (productID)
        this->productID = wcsdup(productID);
    active = 0;
    attached = 1;
    enabled = 0;

#ifdef _MSC_VER
    hWndProc = 0;
#endif

    virtualControls = 0;
    numVirtualControls = 0;
    virtualControlState = 0;
    oldVirtualControlState = 0;

    physicalControls = 0;
    numPhysicalControls = 0;
    physicalControlState = 0;

    ffEffectTypes = 0;
    numFFEffectTypes = 0;
    ffAxes = 0;
    numFFAxes = 0;
}

void Device::FreeState()
{
    if (virtualControlState)
        free(virtualControlState);
    virtualControlState = 0;
    oldVirtualControlState = 0;
    physicalControlState = 0;
}

Device::~Device()
{
    Deactivate();
    // Generally called by deactivate, but just in case...
    FreeState();
    int i;
    for (int port = 0; port < 2; port++) {
        for (int slot = 0; slot < 4; slot++) {
            for (int padtype = 0; padtype < numPadTypes; padtype++) {
                free(pads[port][slot][padtype].bindings);
                for (i = 0; i < pads[port][slot][padtype].numFFBindings; i++) {
                    free(pads[port][slot][padtype].ffBindings[i].axes);
                }
                free(pads[port][slot][padtype].ffBindings);
            }
        }
    }
    free(virtualControls);

    for (i = numPhysicalControls - 1; i >= 0; i--) {
        if (physicalControls[i].name)
            free(physicalControls[i].name);
    }
    free(physicalControls);

    free(displayName);
    free(instanceID);
    free(productID);
    if (ffAxes) {
        for (i = 0; i < numFFAxes; i++) {
            free(ffAxes[i].displayName);
        }
        free(ffAxes);
    }
    if (ffEffectTypes) {
        for (i = 0; i < numFFEffectTypes; i++) {
            free(ffEffectTypes[i].displayName);
            free(ffEffectTypes[i].effectID);
        }
        free(ffEffectTypes);
    }
}

void Device::AddFFEffectType(const wchar_t *displayName, const wchar_t *effectID, EffectType type)
{
    ffEffectTypes = (ForceFeedbackEffectType *)realloc(ffEffectTypes, sizeof(ForceFeedbackEffectType) * (numFFEffectTypes + 1));
    ffEffectTypes[numFFEffectTypes].displayName = wcsdup(displayName);
    ffEffectTypes[numFFEffectTypes].effectID = wcsdup(effectID);
    ffEffectTypes[numFFEffectTypes].type = type;
    numFFEffectTypes++;
}

void Device::AddFFAxis(const wchar_t *displayName, int id)
{
    ffAxes = (ForceFeedbackAxis *)realloc(ffAxes, sizeof(ForceFeedbackAxis) * (numFFAxes + 1));
    ffAxes[numFFAxes].id = id;
    ffAxes[numFFAxes].displayName = wcsdup(displayName);
    numFFAxes++;
    int bindingsExist = 0;
    for (int port = 0; port < 2; port++) {
        for (int slot = 0; slot < 4; slot++) {
            for (int padtype = 0; padtype < numPadTypes; padtype++) {
                for (int i = 0; i < pads[port][slot][padtype].numFFBindings; i++) {
                    ForceFeedbackBinding *b = pads[port][slot][padtype].ffBindings + i;
                    b->axes = (AxisEffectInfo *)realloc(b->axes, sizeof(AxisEffectInfo) * (numFFAxes));
                    memset(b->axes + (numFFAxes - 1), 0, sizeof(AxisEffectInfo));
                    bindingsExist = 1;
                }
            }
        }
    }
    // Generally the case when not loading a binding file.
    if (!bindingsExist) {
        int i = numFFAxes - 1;
        ForceFeedbackAxis temp = ffAxes[i];
        while (i && temp.id < ffAxes[i - 1].id) {
            ffAxes[i] = ffAxes[i - 1];
            i--;
        }
        ffAxes[i] = temp;
    }
}

void Device::AllocState()
{
    FreeState();
    virtualControlState = (int *)calloc(numVirtualControls + numVirtualControls + numPhysicalControls, sizeof(int));
    oldVirtualControlState = virtualControlState + numVirtualControls;
    physicalControlState = oldVirtualControlState + numVirtualControls;
}

void Device::FlipState()
{
    memcpy(oldVirtualControlState, virtualControlState, sizeof(int) * numVirtualControls);
}

void Device::PostRead()
{
    FlipState();
}

void Device::CalcVirtualState()
{
    for (int i = 0; i < numPhysicalControls; i++) {
        PhysicalControl *c = physicalControls + i;
        int index = c->baseVirtualControlIndex;
        int val = physicalControlState[i];
        if (c->type & BUTTON) {
            virtualControlState[index] = val;
            // DirectInput keyboard events only.
            if (this->api == DI && this->type == KEYBOARD) {
                if (!(virtualControlState[index] >> 15) != !(oldVirtualControlState[index] >> 15) && c->vkey) {
                    // Check for alt-F4 to avoid toggling skip mode incorrectly.
                    if (c->vkey == VK_F4) {
                        int i;
                        for (i = 0; i < numPhysicalControls; i++) {
                            if (virtualControlState[physicalControls[i].baseVirtualControlIndex] &&
                                (physicalControls[i].vkey == VK_MENU ||
                                 physicalControls[i].vkey == VK_RMENU ||
                                 physicalControls[i].vkey == VK_LMENU)) {
                                break;
                            }
                        }
                        if (i < numPhysicalControls)
                            continue;
                    }
                    int event = KEYPRESS;
                    if (!(virtualControlState[index] >> 15))
                        event = KEYRELEASE;
                    QueueKeyEvent(c->vkey, event);
                }
            }
        } else if (c->type & ABSAXIS) {
            virtualControlState[index] = (val + FULLY_DOWN) / 2;
            // Positive.  Overkill.
            virtualControlState[index + 1] = (val & ~(val >> 31));
            // Negative
            virtualControlState[index + 2] = (-val & (val >> 31));
        } else if (c->type & RELAXIS) {
            int delta = val - oldVirtualControlState[index];
            virtualControlState[index] = val;
            // Positive
            virtualControlState[index + 1] = (delta & ~(delta >> 31));
            // Negative
            virtualControlState[index + 2] = (-delta & (delta >> 31));
        } else if (c->type & POV) {
            virtualControlState[index] = val;
            int iSouth = 0;
            int iEast = 0;
            if ((unsigned int)val <= 37000) {
                double angle = val * (3.141592653589793 / 18000.0);
                double East = sin(angle);
                double South = -cos(angle);
                // Normalize so greatest direction is 1.
                double mul = FULLY_DOWN / std::max(fabs(South), fabs(East));

                iEast = (int)floor(East * mul + 0.5);
                iSouth = (int)floor(South * mul + 0.5);
            }
            // N
            virtualControlState[index + 1] = (-iSouth & (iSouth >> 31));
            // S
            virtualControlState[index + 3] = (iSouth & ~(iSouth >> 31));
            // E
            virtualControlState[index + 2] = (iEast & ~(iEast >> 31));
            // W
            virtualControlState[index + 4] = (-iEast & (iEast >> 31));
        }
    }
}

VirtualControl *Device::GetVirtualControl(unsigned int uid)
{
    for (int i = 0; i < numVirtualControls; i++) {
        if (virtualControls[i].uid == uid)
            return virtualControls + i;
    }
    return 0;
}

VirtualControl *Device::AddVirtualControl(unsigned int uid, int physicalControlIndex)
{
    // Not really necessary, as always call AllocState when activated, but doesn't hurt.
    FreeState();

    if (numVirtualControls % 16 == 0) {
        virtualControls = (VirtualControl *)realloc(virtualControls, sizeof(VirtualControl) * (numVirtualControls + 16));
    }
    VirtualControl *c = virtualControls + numVirtualControls;

    c->uid = uid;
    c->physicalControlIndex = physicalControlIndex;

    numVirtualControls++;
    return c;
}

PhysicalControl *Device::AddPhysicalControl(ControlType type, unsigned short id, unsigned short vkey, const wchar_t *name)
{
    // Not really necessary, as always call AllocState when activated, but doesn't hurt.
    FreeState();

    if (numPhysicalControls % 16 == 0) {
        physicalControls = (PhysicalControl *)realloc(physicalControls, sizeof(PhysicalControl) * (numPhysicalControls + 16));
    }
    PhysicalControl *control = physicalControls + numPhysicalControls;

    memset(control, 0, sizeof(PhysicalControl));
    control->type = type;
    control->id = id;
    if (name)
        control->name = wcsdup(name);
    control->baseVirtualControlIndex = numVirtualControls;
    unsigned int uid = id | (type << 16);
    if (type & BUTTON) {
        AddVirtualControl(uid, numPhysicalControls);
        control->vkey = vkey;
    } else if (type & AXIS) {
        AddVirtualControl(uid | UID_AXIS, numPhysicalControls);
        AddVirtualControl(uid | UID_AXIS_POS, numPhysicalControls);
        AddVirtualControl(uid | UID_AXIS_NEG, numPhysicalControls);
    } else if (type & POV) {
        AddVirtualControl(uid | UID_POV, numPhysicalControls);
        AddVirtualControl(uid | UID_POV_N, numPhysicalControls);
        AddVirtualControl(uid | UID_POV_E, numPhysicalControls);
        AddVirtualControl(uid | UID_POV_S, numPhysicalControls);
        AddVirtualControl(uid | UID_POV_W, numPhysicalControls);
    }
    numPhysicalControls++;
    return control;
}

void Device::SetEffects(unsigned char port, unsigned int slot, unsigned char motor, unsigned char force)
{
    int padtype = config.padConfigs[port][slot].type;
    for (int i = 0; i < pads[port][slot][padtype].numFFBindings; i++) {
        ForceFeedbackBinding *binding = pads[port][slot][padtype].ffBindings + i;
        if (binding->motor == motor) {
            SetEffect(binding, force);
        }
    }
}

wchar_t *GetDefaultControlName(unsigned short id, int type)
{
    static wchar_t name[20];
    if (type & BUTTON) {
        wsprintfW(name, L"Button %i", id);
    } else if (type & AXIS) {
        wsprintfW(name, L"Axis %i", id);
    } else if (type & POV) {
        wsprintfW(name, L"POV %i", id);
    } else {
        wcscpy(name, L"Unknown");
    }
    return name;
}

wchar_t *Device::GetVirtualControlName(VirtualControl *control)
{
    static wchar_t temp[100];
    wchar_t *baseName = 0;
    if (control->physicalControlIndex >= 0) {
        baseName = physicalControls[control->physicalControlIndex].name;
        if (!baseName)
            baseName = GetPhysicalControlName(&physicalControls[control->physicalControlIndex]);
    }
    unsigned int uid = control->uid;
    if (!baseName) {
        baseName = GetDefaultControlName(uid & 0xFFFF, (uid >> 16) & 0x1F);
    }
    uid &= 0xFF000000;
    int len = (int)wcslen(baseName);
    if (len > 99)
        len = 99;
    memcpy(temp, baseName, len * sizeof(wchar_t));
    temp[len] = 0;
    if (uid) {
        if (len > 95)
            len = 95;
        wchar_t *out = temp + len;
        if (uid == UID_AXIS_POS) {
            wcscpy(out, L" +");
        } else if (uid == UID_AXIS_NEG) {
            wcscpy(out, L" -");
        } else if (uid == UID_POV_N) {
            wcscpy(out, L" N");
        } else if (uid == UID_POV_E) {
            wcscpy(out, L" E");
        } else if (uid == UID_POV_S) {
            wcscpy(out, L" S");
        } else if (uid == UID_POV_W) {
            wcscpy(out, L" W");
        }
    }
    return temp;
}

wchar_t *Device::GetPhysicalControlName(PhysicalControl *control)
{
    if (control->name)
        return control->name;
    return GetDefaultControlName(control->id, control->type);
}
