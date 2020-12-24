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
#include "../Global.h"

std::vector<device_binding_list> dev_bind;

GamepadPanel::GamepadPanel(wxNotebook* parent, unsigned int port, unsigned int slot)
	: wxPanel(parent)
{
	m_port = port;
	m_slot = slot;

	auto* tab_box = new wxBoxSizer(wxHORIZONTAL);
	auto* left_box = new wxBoxSizer(wxVERTICAL);

	pad_list = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(360, 400));
	pad_list->AppendTextColumn("Device", wxDATAVIEW_CELL_INERT, 120);
	pad_list->AppendTextColumn("PC", wxDATAVIEW_CELL_INERT, 120);
	pad_list->AppendTextColumn("PS2", wxDATAVIEW_CELL_INERT, 120);

	Update();
	left_box->Add(pad_list, wxSizerFlags().Expand());
	auto* right_box = new wxBoxSizer(wxVERTICAL);
	status_bar = new wxTextCtrl(this, wxID_ANY, "Gamepad Settings", wxDefaultPosition, wxSize(400, -1), wxTE_READONLY | wxTE_CENTRE);
	right_box->Add(status_bar, wxSizerFlags().Expand());

	auto* control_grid = new wxFlexGridSizer(2, 10, 10);

	auto* shoulder_box = new wxStaticBoxSizer(wxHORIZONTAL, this, "Shoulder Buttons");
	auto* shoulder_grid = new wxFlexGridSizer(2, 5, 5);

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
	auto* select_grid = new wxFlexGridSizer(2, 5, 5);

	auto* select_button = new wxButton(this, wxBTN_PAD_ID_SELECT, "Select");
	auto* start_button = new wxButton(this, wxBTN_PAD_ID_START, "Start");
	auto* analog_button = new wxButton(this, wxBTN_PAD_ID_ANALOG, "Analog");
	auto* mouse_button = new wxButton(this, wxBTN_PAD_ID_MOUSE, "Mouse");
	analog_button->Disable();
	mouse_button->Disable();

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
	auto* l_stick_config_button = new wxButton(this, wxBTN_PAD_ID_L_STICK_CONFIG, "Configure...");

	l_stick_box->Add(l_stick_up_button, wxSizerFlags().Centre());
	l_stick_mid_box->Add(l_stick_left_button);
	l_stick_mid_box->Add(l_stick_right_button);
	l_stick_box->Add(l_stick_mid_box);
	l_stick_box->Add(l_stick_down_button, wxSizerFlags().Centre());
	l_stick_box->Add(l_stick_config_button, wxSizerFlags().Centre().Expand());

	control_grid->Add(l_stick_box);

	auto* r_stick_box = new wxStaticBoxSizer(wxVERTICAL, this, "Right Analog Stick");
	auto* r_stick_mid_box = new wxBoxSizer(wxHORIZONTAL);

	auto* r_stick_up_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_U, "Up");
	auto* r_stick_left_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_L, "Left");
	auto* r_stick_right_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_R, "Right");
	auto* r_stick_down_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_D, "Down");
	auto* r_stick_config_button = new wxButton(this, wxBTN_PAD_ID_R_STICK_CONFIG, "Configure...");

	r_stick_box->Add(r_stick_up_button, wxSizerFlags().Centre());
	r_stick_mid_box->Add(r_stick_left_button);
	r_stick_mid_box->Add(r_stick_right_button);
	r_stick_box->Add(r_stick_mid_box);
	r_stick_box->Add(r_stick_down_button, wxSizerFlags().Centre());
	r_stick_box->Add(r_stick_config_button, wxSizerFlags().Centre().Expand());

	control_grid->Add(r_stick_box);
	right_box->Add(control_grid, wxSizerFlags().Centre().Expand());

	auto* controller_settings_button = new wxButton(this, wxBTN_GAMEPAD_CONFIG, "Gamepad Settings...");
	right_box->Add(controller_settings_button, wxSizerFlags().Centre().Expand());

	auto* delete_button = new wxButton(this, wxBTN_PAD_ID_DELETE, "Delete");
	auto* clear_all_button = new wxButton(this, wxBTN_PAD_ID_CLEAR, "Clear All");
	auto* reset_button = new wxButton(this, wxBTN_PAD_ID_RESET, "Reset To Defaults");
	auto* quick_setup_button = new wxButton(this, wxBTN_PAD_ID_QUICK, "Quick Setup");

	//delete_button->Disable();
	//reset_button->Disable();
	auto* button_box = new wxBoxSizer(wxHORIZONTAL);

	button_box->Add(delete_button);
	button_box->Add(clear_all_button);
	button_box->Add(reset_button);
	button_box->Add(quick_setup_button);
	left_box->Add(button_box, wxSizerFlags().Expand());

	tab_box->Add(left_box, wxSizerFlags().Expand());
	tab_box->Add(right_box, wxSizerFlags().Centre().Expand());

	SetSizerAndFit(tab_box);
	Bind(wxEVT_BUTTON, &GamepadPanel::ButtonPressed, this);
};

void GamepadPanel::Update()
{
	int j = 0;

	pad_list->DeleteAllItems();
	dev_bind.clear();

	for (auto const& it : g_conf.keysym_map[m_port])
	{
		wxVector<wxVariant> data;

		//fprintf(stderr, "PAD %d:KEYSYM 0x%x = %d\n", m_port, it.first, it.second);
		if (it.second < MAX_KEYS)
		{
			data.push_back(wxVariant("Keyboard"));

			data.push_back(wxVariant(wxString(KeyName(m_port, 0, it.first))));
			data.push_back(wxVariant(wxString(pad_labels[it.second])));
			pad_list->AppendItem(data);

			device_binding_list temp;
			temp.keyboard = true;
			temp.device = 0;
			temp.port = m_port;
			temp.slot = m_slot;
			temp.key = it.first;
			temp.value = it.second;
			dev_bind.emplace_back(temp);
		}
	}

	if (device_manager == nullptr) fprintf(stderr, "device_manager == null!\n");
	for (auto const& device : device_manager->devices)
	{
		for (auto& key : all_keys)
		{
			//fprintf(stderr, "Device %d : '%s (%s) (%s)'\n", i, device->m_device_name.c_str(), device->GetBindingName(i), pad_labels[i]);
			wxVector<wxVariant> data;

			data.push_back(wxVariant(device->m_device_name.c_str()));
			data.push_back(wxVariant(wxString(device->GetBindingName(m_port, (int)key))));
			data.push_back(wxVariant(wxString(pad_labels[(int)key])));
			pad_list->AppendItem(data);

			device_binding_list temp;
			temp.keyboard = false;
			temp.device = j;
			temp.port = m_port;
			temp.slot = m_slot;
			temp.key = device->m_bindings[m_port][(int)key];
			temp.value = (int)key;
			dev_bind.emplace_back(temp);
		}
		j++;
	}
	wxYieldIfNeeded();
}

void GamepadPanel::CallUpdate(wxCommandEvent& event)
{
	Update();
}

void GamepadPanel::ClearGamepadKey(gamePadValues pad_key)
{
	// Erase the keyboard binded key
	u32 keysim = m_simulatedKeys[m_port][pad_key];
	m_simulatedKeys[m_port][pad_key] = 0;

	// erase gamepad entry (keysim map)
	if (g_conf.keysym_map[m_port].count(keysim) > 0)
		g_conf.keysym_map[m_port].erase(keysim);
}

void GamepadPanel::ConfigureGamepadKey(gamePadValues pad_key)
{
	bool captured = false;
	u32 key_pressed = 0;

	while (!captured)
	{
		if (PollForNewKeyboardKeys(key_pressed))
		{
			// special case for keyboard/mouse to handle multiple keys
			// Note: key_pressed == UINT32_MAX when ESC is hit to abort the capture
			if (key_pressed != UINT32_MAX)
			{
				ClearGamepadKey(pad_key);

				g_conf.set_keyboard_key(m_port, key_pressed, pad_key);
				m_simulatedKeys[m_port][pad_key] = key_pressed;
			}
			captured = true;
		}
	}
}

void GamepadPanel::DeleteBinding()
{
	int list_idx = pad_list->GetSelectedRow();

	if (list_idx >= 0)
	{
		if (dev_bind[list_idx].keyboard)
		{
			ClearGamepadKey((gamePadValues)dev_bind[list_idx].value);
			Update();
		}
	}
}

void GamepadPanel::ClearAll()
{
	g_conf.keysym_map[m_port].clear();

	for (auto& key : all_keys)
	{
		m_simulatedKeys[m_port][(int)key] = 0;
	}
}

void GamepadPanel::QuickBindings()
{
	for (auto& key : all_keys)
	{
		status_bar->SetValue(wxString::Format("Press a key to set '%s' to, or Escape to cancel.", pad_labels[(int)key]));
		wxYieldIfNeeded();

		ConfigureGamepadKey(key);
		usleep(500000); // give enough time to the user to release the button
	}

	status_bar->SetValue(wxString("Gamepad Configuration."));
}

void GamepadPanel::ResetToDefaults()
{
	DefaultKeyboardValues();
	for (auto& dev : device_manager->devices)
	{
		dev->ClearBindings();
		dev->ResetBindingsToDefault();
	}
	Update();
}
void GamepadPanel::ButtonPressed(wxCommandEvent& event)
{
	wxButton* button = (wxButton*)event.GetEventObject(); // get the button object
	int button_id = button->GetId();

	if (btn_to_pad.count(button_id) > 0)
	{
		gamePadValues pad_key = btn_to_pad[button_id];

		status_bar->SetValue(wxString::Format("Press a key to set '%s' to, or Escape to cancel.", pad_labels[pad_key]));
		wxYieldIfNeeded();

		ConfigureGamepadKey(pad_key);

		status_bar->SetValue(wxString("Gamepad Configuration."));
	}

	if (button_id == wxBTN_PAD_ID_DELETE)
		DeleteBinding();
	if (button_id == wxBTN_PAD_ID_QUICK)
		QuickBindings();
	if (button_id == wxBTN_PAD_ID_CLEAR)
		ClearAll();
	if (button_id == wxBTN_PAD_ID_RESET)
		ResetToDefaults();

	if (button_id == wxBTN_PAD_ID_L_STICK_CONFIG)
	{
		JoystickConfiguration joystick_config(m_port, true, this);

		joystick_config.InitJoystickConfiguration();
		joystick_config.ShowModal();
	}

	if (button_id == wxBTN_PAD_ID_R_STICK_CONFIG)
	{
		JoystickConfiguration joystick_config(m_port, false, this);

		joystick_config.InitJoystickConfiguration();
		joystick_config.ShowModal();
	}

	if (button_id == wxBTN_GAMEPAD_CONFIG)
	{
		GamepadConfiguration gamepad_config(m_port, this);

		gamepad_config.InitGamepadConfiguration();
		gamepad_config.ShowModal();
	}

	Update();
}

void GamepadPanel::Populate(int port, int slot, int padtype)
{
}