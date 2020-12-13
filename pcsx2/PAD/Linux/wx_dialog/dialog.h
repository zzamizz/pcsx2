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

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/effects.h>
#include <wx/rawbmp.h>
#include <wx/graphics.h>
#include <wx/timer.h>
#include <wx/dataview.h>

#include <string>

#include "../Device.h"
#include "../keyboard.h"
#include "../PAD.h"
#include "GamepadPanel.h"
#include "opPanel.h"

#include "GamepadConfiguration.h"
#include "JoystickConfiguration.h"

// Allow to found quickly button id
// e.g L2 → 0, triangle → 4, ...
// see PAD.h for more details about gamepad button id

enum gui_buttons
{
	Analog = PAD_R_LEFT + 1, // Analog button (not yet supported ?)
	JoyL_config,             // Left Joystick Configuration
	JoyR_config,             // Right Joystick Configuration
	Gamepad_config,          // Gamepad Configuration
	Set_all                  // Set all buttons
};

#define BUTTONS_LENGHT 29 // numbers of buttons on the gamepad
#define UPDATE_TIME 5
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 740

[[maybe_unused]]static const char* padTypes[] = {
	"Unplugged",
	"Dualshock 2",
	"Guitar",
	"Pop'n Music controller",
	"PS1 Mouse",
	"neGcon"};

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
	opPanel* m_pan_tabs[GAMEPAD_NUMBER]; // Gamepad Tabs box
	std::vector<GamepadPanel*> m_gamepad_tabs;
	// Notebooks
	wxNotebook* m_tab_gamepad; // Joysticks Tabs
	// Buttons
	wxButton* m_bt_gamepad[GAMEPAD_NUMBER][BUTTONS_LENGHT]; // Joystick button use to modify the button mapping

	// Timer
	wxTimer m_time_update_gui;
	// Check if the gui must display feddback image
	bool m_pressed[GAMEPAD_NUMBER][NB_IMG];

	// methods
	void config_key(int, int);
	void clear_key(int, int);
	void repopulate();
	void set_padding();

	// Events
	void OnButtonClicked(wxCommandEvent&);
	void JoystickEvent(wxTimerEvent&);

	// Make a member variable temporarily.
	int padding[BUTTONS_LENGHT][4];

public:
	PADDialog();
	void InitDialog();
	void show();
	void Update();
	GeneralPanel* general_panel;
};

extern void DisplayDialog(); // Main function

// Contain all simulated key
extern u32 m_simulatedKeys[GAMEPAD_NUMBER][MAX_KEYS];;

#endif // __DIALOG_H__
