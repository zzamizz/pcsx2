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

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/dataview.h>

#include "../keyboard.h"

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
};

enum
{
	wxBTN_PAD_ID_MOUSE = wxID_HIGHEST + 1,
	wxBTN_PAD_ID_SELECT = wxID_HIGHEST + 2,
	wxBTN_PAD_ID_L3 = wxID_HIGHEST + 3,
	wxBTN_PAD_ID_R3 = wxID_HIGHEST + 4,
	wxBTN_PAD_ID_START = wxID_HIGHEST + 5,

	wxBTN_PAD_ID_D_PAD_U = wxID_HIGHEST + 6,
	wxBTN_PAD_ID_D_PAD_R = wxID_HIGHEST + 7,
	wxBTN_PAD_ID_D_PAD_D = wxID_HIGHEST + 8,
	wxBTN_PAD_ID_D_PAD_L = wxID_HIGHEST + 9,

	wxBTN_PAD_ID_L2 = wxID_HIGHEST + 10,
	wxBTN_PAD_ID_R2 = wxID_HIGHEST + 11,
	wxBTN_PAD_ID_L1 = wxID_HIGHEST + 12,
	wxBTN_PAD_ID_R1 = wxID_HIGHEST + 13,

	wxBTN_PAD_ID_TRIANGLE = wxID_HIGHEST + 14,
	wxBTN_PAD_ID_CIRCLE = wxID_HIGHEST + 15,
	wxBTN_PAD_ID_SQUARE = wxID_HIGHEST + 16,
	wxBTN_PAD_ID_CROSS = wxID_HIGHEST + 17,

	wxBTN_PAD_ID_L_STICK_U = wxID_HIGHEST + 18,
	wxBTN_PAD_ID_L_STICK_R = wxID_HIGHEST + 19,
	wxBTN_PAD_ID_L_STICK_D = wxID_HIGHEST + 20,
	wxBTN_PAD_ID_L_STICK_L = wxID_HIGHEST + 21,

	wxBTN_PAD_ID_R_STICK_U = wxID_HIGHEST + 22,
	wxBTN_PAD_ID_R_STICK_R = wxID_HIGHEST + 23,
	wxBTN_PAD_ID_R_STICK_D = wxID_HIGHEST + 24,
	wxBTN_PAD_ID_R_STICK_L = wxID_HIGHEST + 25,

	wxBTN_PAD_ID_ANALOG = wxID_HIGHEST + 26,
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

/*enum gamePadValues
{
	PAD_L2 = 0,   // L2 button
	PAD_R2,       // R2 button
	PAD_L1,       // L1 button
	PAD_R1,       // R1 button
	PAD_TRIANGLE, // Triangle button ▲
	PAD_CIRCLE,   // Circle button ●
	PAD_CROSS,    // Cross button ✖
	PAD_SQUARE,   // Square button ■
	PAD_SELECT,   // Select button
	PAD_L3,       // Left joystick button (L3)
	PAD_R3,       // Right joystick button (R3)
	PAD_START,    // Start button
	PAD_UP,       // Directional pad ↑
	PAD_RIGHT,    // Directional pad →
	PAD_DOWN,     // Directional pad ↓
	PAD_LEFT,     // Directional pad ←
	PAD_L_UP,     // Left joystick (Up) ↑
	PAD_L_RIGHT,  // Left joystick (Right) →
	PAD_L_DOWN,   // Left joystick (Down) ↓
	PAD_L_LEFT,   // Left joystick (Left) ←
	PAD_R_UP,     // Right joystick (Up) ↑
	PAD_R_RIGHT,  // Right joystick (Right) →
	PAD_R_DOWN,   // Right joystick (Down) ↓
	PAD_R_LEFT    // Right joystick (Left) ←
};*/