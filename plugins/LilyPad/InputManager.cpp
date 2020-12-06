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
#include "InputManager.h"
#include "KeyboardQueue.h"
#include "Config.h"

InputDeviceManager *dm = 0;

InputDeviceManager::InputDeviceManager()
{
    ClearDevices();
}

void InputDeviceManager::ClearDevices()
{
    devices.clear();
    numDevices = 0;
}

InputDeviceManager::~InputDeviceManager()
{
    ClearDevices();
}

void InputDeviceManager::AddDevice(Device *d)
{
    devices.emplace_back(d);
    numDevices++;
}

void InputDeviceManager::Update(InitInfo *info)
{
    for (int i = 0; i < numDevices; i++) {
        if (devices[i]->enabled) {
            if (!devices[i]->active) {
                if (!devices[i]->Activate(info) || !devices[i]->Update())
                    continue;
                devices[i]->CalcVirtualState();
                devices[i]->PostRead();
            }
            if (devices[i]->Update())
                devices[i]->CalcVirtualState();
        }
    }
}

void InputDeviceManager::PostRead()
{
    for (int i = 0; i < numDevices; i++) {
        if (devices[i]->active)
            devices[i]->PostRead();
    }
}

Device *InputDeviceManager::GetActiveDevice(InitInfo *info, unsigned int *uid, int *index, int *value)
{
    int i, j;
    Update(info);
    int bestDiff = FULLY_DOWN / 2;
    Device *bestDevice = 0;
    for (i = 0; i < numDevices; i++) {
        if (devices[i]->active) {
            for (j = 0; j < devices[i]->numVirtualControls; j++) {
                if (devices[i]->virtualControlState[j] == devices[i]->oldVirtualControlState[j])
                    continue;
                if (devices[i]->virtualControls[j].uid & UID_POV)
                    continue;
                // Fix for releasing button used to click on bind button
                if (!((devices[i]->virtualControls[j].uid >> 16) & (POV | RELAXIS | ABSAXIS))) {
                    if (abs(devices[i]->oldVirtualControlState[j]) > abs(devices[i]->virtualControlState[j])) {
                        devices[i]->oldVirtualControlState[j] = 0;
                    }
                }
                int diff = abs(devices[i]->virtualControlState[j] - devices[i]->oldVirtualControlState[j]);
                // Make it require a bit more work to bind relative axes.
                if (((devices[i]->virtualControls[j].uid >> 16) & 0xFF) == RELAXIS) {
                    diff = diff / 4 + 1;
                }
                // Less pressure needed to bind DS3/SCP buttons.
                if ((devices[i]->api == DS3 || devices[i]->api == XINPUT) && (((devices[i]->virtualControls[j].uid >> 16) & 0xFF) & BUTTON)) {
                    diff *= 4;
                }
                if (diff > bestDiff) {
                    if (devices[i]->virtualControls[j].uid & UID_AXIS) {
                        if ((((devices[i]->virtualControls[j].uid >> 16) & 0xFF) != ABSAXIS))
                            continue;
                        // Very picky when binding entire axes.  Prefer binding half-axes.
                        if (!((devices[i]->oldVirtualControlState[j] < FULLY_DOWN / 32 && devices[i]->virtualControlState[j] > FULLY_DOWN / 8) ||
                              (devices[i]->oldVirtualControlState[j] > 31 * FULLY_DOWN / 32 && devices[i]->virtualControlState[j] < 7 * FULLY_DOWN / 8))) {
                            continue;
                        }
                    } else if ((((devices[i]->virtualControls[j].uid >> 16) & 0xFF) == ABSAXIS)) {
                        if (devices[i]->oldVirtualControlState[j] > 15 * FULLY_DOWN / 16)
                            continue;
                    }
                    bestDiff = diff;
                    *uid = devices[i]->virtualControls[j].uid;
                    *index = j;
                    bestDevice = devices[i];
                    if (value) {
                        if ((devices[i]->virtualControls[j].uid >> 16) & RELAXIS) {
                            *value = devices[i]->virtualControlState[j] - devices[i]->oldVirtualControlState[j];
                        } else {
                            *value = devices[i]->virtualControlState[j];
                        }
                    }
                }
            }
        }
    }
    // Don't call when binding.
    // PostRead();
    return bestDevice;
}

void InputDeviceManager::ReleaseInput()
{
    for (int i = 0; i < numDevices; i++) {
        if (devices[i]->active)
            devices[i]->Deactivate();
    }
}

void InputDeviceManager::EnableDevices(DeviceType type, DeviceAPI api)
{
    for (int i = 0; i < numDevices; i++) {
        if (devices[i]->api == api && devices[i]->type == type) {
            EnableDevice(i);
        }
    }
}

void InputDeviceManager::DisableAllDevices()
{
    for (int i = 0; i < numDevices; i++) {
        DisableDevice(i);
    }
}

void InputDeviceManager::DisableDevice(int index)
{
    devices[index]->enabled = 0;
    if (devices[index]->active) {
        devices[index]->Deactivate();
    }
}

ForceFeedbackEffectType *Device::GetForcefeedbackEffect(wchar_t *id)
{
    for (int i = 0; i < numFFEffectTypes; i++) {
        if (!wcsicmp(id, ffEffectTypes[i].effectID)) {
            return &ffEffectTypes[i];
        }
    }
    return 0;
}

ForceFeedbackAxis *Device::GetForceFeedbackAxis(int id)
{
    for (int i = 0; i < numFFAxes; i++) {
        if (ffAxes[i].id == id)
            return &ffAxes[i];
    }
    return 0;
}

void InputDeviceManager::CopyBindings(int numOldDevices, std::vector<Device*> oldDevices)
{
    int *oldMatches = (int *)malloc(sizeof(int) * numOldDevices);
    int *matches = (int *)malloc(sizeof(int) * numDevices);
    int i, j, port, slot;
    Device *old, *dev;
    for (i = 0; i < numDevices; i++) {
        matches[i] = -1;
    }
    for (i = 0; i < numOldDevices; i++) {
        oldMatches[i] = -2;
        old = oldDevices[i];
        for (port = 0; port < 2; port++) {
            for (slot = 0; slot < 4; slot++) {
                for (int padtype = 0; padtype < numPadTypes; padtype++) {
                    if (old->pads[port][slot][padtype].numBindings + old->pads[port][slot][padtype].numFFBindings) {
                        // Means that there are bindings.
                        oldMatches[i] = -1;
                    }
                }
            }
        }
    }
    // Loops through ids looking for match, from most specific to most general.
    for (int id = 0; id < 3; id++) {
        for (i = 0; i < numOldDevices; i++) {
            if (oldMatches[i] >= 0)
                continue;
            for (j = 0; j < numDevices; j++) {
                if (matches[j] >= 0) {
                    continue;
                }
                wchar_t *id1 = devices[j]->IDs[id];
                wchar_t *id2 = oldDevices[i]->IDs[id];
                if (!id1 || !id2) {
                    continue;
                }
                if (!wcsicmp(id1, id2)) {
                    matches[j] = i;
                    oldMatches[i] = j;
                    break;
                }
            }
        }
    }

    for (i = 0; i < numOldDevices; i++) {
        if (oldMatches[i] == -2)
            continue;
        old = oldDevices[i];
        if (oldMatches[i] < 0) {
            dev = new Device(old->api, old->type, old->displayName, old->instanceID, old->productID);
            dev->attached = 0;
            AddDevice(dev);
            for (j = 0; j < old->numVirtualControls; j++) {
                VirtualControl *c = old->virtualControls + j;
                dev->AddVirtualControl(c->uid, -1);
            }
            for (j = 0; j < old->numFFEffectTypes; j++) {
                ForceFeedbackEffectType *effect = old->ffEffectTypes + j;
                dev->AddFFEffectType(effect->displayName, effect->effectID, effect->type);
            }
            for (j = 0; j < old->numFFAxes; j++) {
                ForceFeedbackAxis *axis = old->ffAxes + j;
                dev->AddFFAxis(axis->displayName, axis->id);
            }
            // Just steal the old bindings directly when there's no matching device.
            // Indices will be the same.
            memcpy(dev->pads, old->pads, sizeof(old->pads));
            memset(old->pads, 0, sizeof(old->pads));
        } else {
            dev = devices[oldMatches[i]];
            for (port = 0; port < 2; port++) {
                for (slot = 0; slot < 4; slot++) {
                    for (int padtype = 0; padtype < numPadTypes; padtype++) {
                        if (old->pads[port][slot][padtype].numBindings) {
                            dev->pads[port][slot][padtype].bindings = (Binding *)malloc(old->pads[port][slot][padtype].numBindings * sizeof(Binding));
                            for (int j = 0; j < old->pads[port][slot][padtype].numBindings; j++) {
                                Binding *bo = old->pads[port][slot][padtype].bindings + j;
                                Binding *bn = dev->pads[port][slot][padtype].bindings + dev->pads[port][slot][padtype].numBindings;
                                VirtualControl *cn = dev->GetVirtualControl(old->virtualControls[bo->controlIndex].uid);
                                if (cn) {
                                    *bn = *bo;
                                    bn->controlIndex = cn - dev->virtualControls;
                                    dev->pads[port][slot][padtype].numBindings++;
                                }
                            }
                        }
                        if (old->pads[port][slot][padtype].numFFBindings) {
                            dev->pads[port][slot][padtype].ffBindings = (ForceFeedbackBinding *)malloc(old->pads[port][slot][padtype].numFFBindings * sizeof(ForceFeedbackBinding));
                            for (int j = 0; j < old->pads[port][slot][padtype].numFFBindings; j++) {
                                ForceFeedbackBinding *bo = old->pads[port][slot][padtype].ffBindings + j;
                                ForceFeedbackBinding *bn = dev->pads[port][slot][padtype].ffBindings + dev->pads[port][slot][padtype].numFFBindings;
                                ForceFeedbackEffectType *en = dev->GetForcefeedbackEffect(old->ffEffectTypes[bo->effectIndex].effectID);
                                if (en) {
                                    *bn = *bo;
                                    bn->effectIndex = en - dev->ffEffectTypes;
                                    bn->axes = (AxisEffectInfo *)calloc(dev->numFFAxes, sizeof(AxisEffectInfo));
                                    for (int k = 0; k < old->numFFAxes; k++) {
                                        ForceFeedbackAxis *newAxis = dev->GetForceFeedbackAxis(old->ffAxes[k].id);
                                        if (newAxis) {
                                            bn->axes[newAxis - dev->ffAxes] = bo->axes[k];
                                        }
                                    }
                                    dev->pads[port][slot][padtype].numFFBindings++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    free(oldMatches);
    free(matches);
}

void InputDeviceManager::SetEffect(unsigned char port, unsigned int slot, unsigned char motor, unsigned char force)
{
    for (int i = 0; i < numDevices; i++) {
        Device *dev = devices[i];
        if (dev->enabled && dev->numFFEffectTypes) {
            dev->SetEffects(port, slot, motor, force);
        }
    }
}
