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

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/dataview.h>

#include "../Devices/KeyboardDevice.h"
#include "JoystickConfiguration.h"
#include "GamepadConfiguration.h"

extern u32 m_simulatedKeys[GAMEPAD_NUMBER][MAX_KEYS];

class GamepadPanel : public wxPanel
{
private:
	wxDataViewListCtrl* pad_list;
	wxTextCtrl* status_bar;
    unsigned int m_port;
    unsigned int m_slot;

public:
	GamepadPanel(wxNotebook* parent, unsigned int port, unsigned int slot);
	void Update();
	void CallUpdate(wxCommandEvent& event);
	void Populate(int port, int slot, int padtype);
    void ButtonPressed(wxCommandEvent& event);
    void ConfigureGamepadKey(gamePadValues pad_key);
    void ClearGamepadKey(gamePadValues pad_key);
    void ClearAllKeys(gamePadValues pad_key);
    void DeleteBinding();
    void ClearAll();
	void ResetToDefaults();
    void QuickBindings();
};

enum
{
	wxBTN_PAD_ID_MOUSE = wxID_HIGHEST + 1,
	wxBTN_PAD_ID_SELECT,
	wxBTN_PAD_ID_L3,
	wxBTN_PAD_ID_R3,
	wxBTN_PAD_ID_START,

	wxBTN_PAD_ID_D_PAD_U,
	wxBTN_PAD_ID_D_PAD_R,
	wxBTN_PAD_ID_D_PAD_D,
	wxBTN_PAD_ID_D_PAD_L,

	wxBTN_PAD_ID_L2,
	wxBTN_PAD_ID_R2,
	wxBTN_PAD_ID_L1,
	wxBTN_PAD_ID_R1,

	wxBTN_PAD_ID_TRIANGLE,
	wxBTN_PAD_ID_CIRCLE,
	wxBTN_PAD_ID_SQUARE,
	wxBTN_PAD_ID_CROSS,

	wxBTN_PAD_ID_L_STICK_U,
	wxBTN_PAD_ID_L_STICK_R,
	wxBTN_PAD_ID_L_STICK_D,
	wxBTN_PAD_ID_L_STICK_L,
	wxBTN_PAD_ID_L_STICK_CONFIG,

	wxBTN_PAD_ID_R_STICK_U,
	wxBTN_PAD_ID_R_STICK_R,
	wxBTN_PAD_ID_R_STICK_D,
	wxBTN_PAD_ID_R_STICK_L,
	wxBTN_PAD_ID_R_STICK_CONFIG,

	wxBTN_GAMEPAD_CONFIG,

	wxBTN_PAD_ID_ANALOG,
    wxBTN_PAD_ID_DELETE,
    wxBTN_PAD_ID_CLEAR,
    wxBTN_PAD_ID_QUICK,
	wxBTN_PAD_ID_RESET
};

[[maybe_unused]]static std::map<int, gamePadValues> btn_to_pad =
{
	//wxBTN_PAD_ID_MOUSE, 
	{wxBTN_PAD_ID_SELECT, PAD_SELECT},
	{wxBTN_PAD_ID_L3, PAD_L3},
	{wxBTN_PAD_ID_R3, PAD_R3},
	{wxBTN_PAD_ID_START, PAD_START},

	{wxBTN_PAD_ID_D_PAD_U, PAD_UP},
	{wxBTN_PAD_ID_D_PAD_R, PAD_RIGHT},
	{wxBTN_PAD_ID_D_PAD_D, PAD_DOWN},
	{wxBTN_PAD_ID_D_PAD_L, PAD_LEFT},

	{wxBTN_PAD_ID_L2, PAD_L2},
	{wxBTN_PAD_ID_R2, PAD_R2},
	{wxBTN_PAD_ID_L1, PAD_L1},
	{wxBTN_PAD_ID_R1, PAD_R1},

	{wxBTN_PAD_ID_TRIANGLE, PAD_TRIANGLE},
	{wxBTN_PAD_ID_CIRCLE, PAD_CIRCLE},
	{wxBTN_PAD_ID_SQUARE, PAD_SQUARE},
	{wxBTN_PAD_ID_CROSS, PAD_CROSS},

	{wxBTN_PAD_ID_L_STICK_U, PAD_L_UP},
	{wxBTN_PAD_ID_L_STICK_R, PAD_L_RIGHT},
	{wxBTN_PAD_ID_L_STICK_D, PAD_L_DOWN},
	{wxBTN_PAD_ID_L_STICK_L, PAD_L_LEFT},

	{wxBTN_PAD_ID_R_STICK_U, PAD_R_UP},
	{wxBTN_PAD_ID_R_STICK_R, PAD_R_RIGHT},
	{wxBTN_PAD_ID_R_STICK_D, PAD_R_DOWN},
	{wxBTN_PAD_ID_R_STICK_L, PAD_R_LEFT}
};

[[maybe_unused]]static const char* pad_labels[] =
	{
		"L2",
		"R2",
		"L1",
		"R1",
		"Triangle",
		"Circle",
		"Cross",
		"Square",
		"Select",
		"L3",
		"R3",
		"Start",
		"D-Pad Up",
		"D-Pad Right",
		"D-Pad Down",
		"D-Pad Left",
		"L-Stick Up",
		"L-Stick Right",
		"L-Stick Down",
		"L-Stick Left",
		"R-Stick Up",
		"R-Stick Right",
		"R-Stick Down",
		"R-Stick Left"
};

struct device_binding_list
{
	bool keyboard;
	int device;
	unsigned int port;
	unsigned int slot;
	int key;
	int value;
};

extern std::vector<device_binding_list> dev_bind;
