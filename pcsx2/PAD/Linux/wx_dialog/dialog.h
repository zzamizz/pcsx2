/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2020  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include <string>

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dataview.h>

#include "../InputManager.h"
#include "../Devices/KeyboardDevice.h"
#include "../Global.h"

#include "GamepadPanel.h"
#include "GamepadConfiguration.h"
#include "JoystickConfiguration.h"

[[maybe_unused]]static const char* padTypes[] = {
	"Unplugged",
	"Dualshock 2",
	"Guitar",
	"Pop'n Music controller",
	"PS1 Mouse",
	"neGcon"};

// Returns 0 if pad doesn't exist due to mtap settings, as a convenience.
static int GetPadName(wxString &string, unsigned int port, unsigned int slot)
{
    if (!slot && !g_conf.multitap[port]) {
        string = wxString::Format(L"Pad %i", port + 1);

    } else {
        string = wxString::Format(L"Pad %i%c", port + 1, 'A' + slot);

        if (!g_conf.multitap[port]) return 0;
    }
    return 1;
}

class GeneralPanel : public wxPanel
{
private:
	wxCheckBox *multitap_1_check, *multitap_2_check, *multiple_bindings_check;
	wxChoice* choice;
	wxDataViewListCtrl* pad_list;

public:
	GeneralPanel(wxWindow* parent);
	~GeneralPanel();
	void RefreshList();
	void CallCheck(wxCommandEvent& /*event*/);
};

class PADDialog : public wxDialog
{
	// Panels
	std::vector<GamepadPanel*> m_gamepad_tabs;

	// Notebooks
	wxNotebook* m_tab_gamepad; // Joysticks Tabs

	// Events
	void OnButtonClicked(wxCommandEvent&);
	void JoystickEvent(wxTimerEvent&);
	void OnWindowShown(wxShowEvent&);

public:
	PADDialog();
	void InitDialog();
	void show();
	void Update();
	GeneralPanel* general_panel;
};

extern void DisplayDialog(); // Main function

// Contain all simulated key
extern u32 m_simulatedKeys[GAMEPAD_NUMBER][MAX_KEYS];

#endif // __DIALOG_H__
