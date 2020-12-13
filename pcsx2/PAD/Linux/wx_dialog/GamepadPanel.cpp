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

#include "GamepadPanel.h"
#include "../PAD.h"

GamepadPanel::GamepadPanel(wxNotebook* parent, unsigned int port, unsigned int slot)
	: wxPanel(parent)
{
	auto* tab_box = new wxBoxSizer(wxHORIZONTAL);
    wxString title;
    GetPadName(title, port, slot);

    pad_list = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(360,200));
    pad_list->AppendTextColumn("Device", wxDATAVIEW_CELL_INERT, 120);
    pad_list->AppendTextColumn("PC", wxDATAVIEW_CELL_INERT, 120);
    pad_list->AppendTextColumn("PS2", wxDATAVIEW_CELL_INERT, 120);

    Update();
    tab_box->Add(pad_list);
    auto* right_box = new wxBoxSizer(wxVERTICAL);
    status_bar = new wxTextCtrl(this, wxID_ANY, "Gamepad Settings", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE);
    right_box->Add(status_bar, wxSizerFlags().Expand());

    auto* control_grid = new wxFlexGridSizer(2, 0, 0);

    auto* shoulder_box = new wxStaticBoxSizer(wxHORIZONTAL, this, "Shoulder Buttons");
    auto* shoulder_grid = new wxFlexGridSizer(2, 0, 0);

    auto* l1_button = new wxButton(this, wxBTN_PAD_ID_L1, "L1");
    auto* r1_button = new wxButton(this, wxBTN_PAD_ID_R1, "R1");
    auto* l2_button = new wxButton(this, wxBTN_PAD_ID_L2, "L2");
    auto* r2_button = new wxButton(this, wxBTN_PAD_ID_R2, "R2");

    shoulder_grid->Add(l1_button);
    shoulder_grid->Add(r1_button);
    shoulder_grid->Add(l2_button);
    shoulder_grid->Add(r2_button);

    shoulder_box->Add(shoulder_grid);
    control_grid->Add(shoulder_box);

    auto* select_box = new wxStaticBoxSizer(wxHORIZONTAL, this);
    auto* select_grid = new wxFlexGridSizer(2, 0, 0);

    auto* select_button = new wxButton(this, wxBTN_PAD_ID_SELECT, "Select");
    auto* start_button = new wxButton(this, wxBTN_PAD_ID_START, "Start");
    auto* analog_button = new wxButton(this, wxBTN_PAD_ID_ANALOG, "Analog");
    auto* mouse_button = new wxButton(this, wxBTN_PAD_ID_MOUSE, "Mouse");

    select_grid->Add(select_button);
    select_grid->Add(start_button);
    select_grid->Add(analog_button);
    select_grid->Add(mouse_button);

    select_box->Add(select_grid);
    control_grid->Add(select_box);

    auto* d_box = new wxStaticBoxSizer(wxVERTICAL, this, "D-Pad");
    auto* mid_d_box = new wxBoxSizer(wxHORIZONTAL);

    auto* up_button = new wxButton(this, wxBTN_PAD_ID_D_PAD_U, "Up");
    auto* left_button = new wxButton(this, wxBTN_PAD_ID_D_PAD_L, "Left");
    auto* right_button = new wxButton(this, wxBTN_PAD_ID_D_PAD_R, "Right");
    auto* down_button = new wxButton(this, wxBTN_PAD_ID_D_PAD_D, "Down");

    d_box->Add(up_button, wxSizerFlags().Centre());
    mid_d_box->Add(left_button);
    mid_d_box->Add(right_button);
    d_box->Add(mid_d_box);
    d_box->Add(down_button, wxSizerFlags().Centre());

    control_grid->Add(d_box);

    auto* face_box = new wxStaticBoxSizer(wxVERTICAL, this, "Face Buttons");
    auto* face_mid_box = new wxBoxSizer(wxHORIZONTAL);

    auto* triangle_button = new wxButton(this, wxBTN_PAD_ID_TRIANGLE, "Triangle");
    auto* square_button = new wxButton(this, wxBTN_PAD_ID_SQUARE, "Square");
    auto* circle_button = new wxButton(this, wxBTN_PAD_ID_CIRCLE, "Circle");
    auto* cross_button = new wxButton(this, wxBTN_PAD_ID_CROSS, "Cross");

    face_box->Add(triangle_button, wxSizerFlags().Centre());
    face_mid_box->Add(square_button);
    face_mid_box->Add(circle_button);
    face_box->Add(face_mid_box);
    face_box->Add(cross_button, wxSizerFlags().Centre());

    control_grid->Add(face_box);

    auto* l_stick_box = new wxStaticBoxSizer(wxVERTICAL, this, "Left Analog Stick");
    auto* l_stick_mid_box = new wxBoxSizer(wxHORIZONTAL);

    auto* l_stick_up_button = new wxButton(this, wxBTN_PAD_ID_L_STICK_U, "Up");
    auto* l_stick_left_button = new wxButton(this, wxBTN_PAD_ID_L_STICK_L, "Left");
    auto* l_stick_right_button = new wxButton(this, wxBTN_PAD_ID_L_STICK_R, "Right");
    auto* l_stick_down_button = new wxButton(this, wxBTN_PAD_ID_L_STICK_D, "Down");

    l_stick_box->Add(l_stick_up_button, wxSizerFlags().Centre());
    l_stick_mid_box->Add(l_stick_left_button);
    l_stick_mid_box->Add(l_stick_right_button);
    l_stick_box->Add(l_stick_mid_box);
    l_stick_box->Add(l_stick_down_button, wxSizerFlags().Centre());

    control_grid->Add(l_stick_box);

    auto* r_stick_box = new wxStaticBoxSizer(wxVERTICAL, this, "Left Analog Stick");
    auto* r_stick_mid_box = new wxBoxSizer(wxHORIZONTAL);

    auto* r_stick_up_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_U, "Up");
    auto* r_stick_left_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_L, "Left");
    auto* r_stick_right_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_R, "Right");
    auto* r_stick_down_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_D, "Down");

    r_stick_box->Add(r_stick_up_button, wxSizerFlags().Centre());
    r_stick_mid_box->Add(r_stick_left_button);
    r_stick_mid_box->Add(r_stick_right_button);
    r_stick_box->Add(r_stick_mid_box);
    r_stick_box->Add(r_stick_down_button, wxSizerFlags().Centre());

    control_grid->Add(r_stick_box);
    right_box->Add(control_grid, wxSizerFlags().Centre().Expand());
    tab_box->Add(right_box, wxSizerFlags().Centre().Expand());

	SetSizerAndFit(tab_box);
	//Bind(wxEVT_BUTTON, &PadTab::CallUpdate, this);
};

void GamepadPanel::Update()
{
}

void GamepadPanel::CallUpdate(wxCommandEvent& event)
{
}

void GamepadPanel::Populate(int port, int slot, int padtype)
{
}