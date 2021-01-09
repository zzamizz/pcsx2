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

#include "dialog.h"

extern void PADLoadConfig();
extern void PADSaveConfig();

// Contain all simulated keys
u32 m_simulatedKeys[GAMEPAD_NUMBER][MAX_KEYS];

static void SysMessage(const char* fmt, ...)
{
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (msg[strlen(msg) - 1] == '\n')
		msg[strlen(msg) - 1] = 0;

	wxMessageDialog dialog(nullptr, msg, "Info", wxOK);
	dialog.ShowModal();
}

GeneralPanel::GeneralPanel(wxWindow* parent) 
	: wxPanel(parent, wxID_ANY)
{
	joy_choices.Clear();
	joy_choices.Add("None");
	for (const auto& j : device_manager->devices)
	{
		joy_choices.Add(j->GetName());
	}

	auto* tab_box = new wxBoxSizer(wxHORIZONTAL);
    auto* pad1_box = new wxStaticBoxSizer(wxVERTICAL, this, "Pad 1");
    auto* pad2_box = new wxStaticBoxSizer(wxVERTICAL, this, "Pad 2");

    multitap_1_check = new wxCheckBox(this, wxID_ANY, "Port 1 Multitap");
	pad1_joy_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(250,-1), joy_choices);
	pad1_rumble_check = new wxCheckBox(this, wxID_ANY, _T("&Enable rumble"));

	auto* rumble1_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Rumble intensity"));
	pad1_rumble_intensity = new wxSlider(this, wxID_ANY, 0, 0, 0x7FFF, wxDefaultPosition, wxSize(250,-1),
										 wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
	rumble1_box->Add(pad1_rumble_intensity);

	auto* joy1_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Joystick sensitivity"));
	pad1_joy_sensitivity = new wxSlider(this, wxID_ANY, 0, 0, 200, wxDefaultPosition, wxSize(250,-1),
											 wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
	joy1_box->Add(pad1_joy_sensitivity);

    pad1_box->Add(multitap_1_check);
    pad1_box->Add(pad1_joy_choice);
    pad1_box->Add(pad1_rumble_check);
    pad1_box->Add(rumble1_box);
    pad1_box->Add(joy1_box);

	tab_box->Add(pad1_box, wxSizerFlags().Border(wxALL, 10));

    multitap_2_check = new wxCheckBox(this, wxID_ANY, "Port 2 Multitap");
	pad2_joy_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(250,-1), joy_choices);
	pad2_rumble_check = new wxCheckBox(this, enable_rumble_id, _T("&Enable rumble"));

	auto* rumble2_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Rumble intensity"));
	pad2_rumble_intensity = new wxSlider(this, wxID_ANY, 0, 0, 0x7FFF, wxDefaultPosition, wxSize(250,-1),
										 wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
	rumble2_box->Add(pad2_rumble_intensity);

	auto* joy2_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Joystick sensitivity"));
	pad2_joy_sensitivity = new wxSlider(this, wxID_ANY, 0, 0, 200, wxDefaultPosition, wxSize(250,-1),
											 wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
	joy2_box->Add(pad2_joy_sensitivity);

    pad2_box->Add(multitap_2_check);
    pad2_box->Add(pad2_joy_choice);
    pad2_box->Add(pad2_rumble_check);
    pad2_box->Add(rumble2_box);
    pad2_box->Add(joy2_box);

	tab_box->Add(pad2_box, wxSizerFlags().Border(wxALL, 10));

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHECKBOX, &GeneralPanel::CallCheck, this);

	Populate();
};

void GeneralPanel::Populate()
{
	u32 pad1_uid = Device::uid_to_index(g_conf.get_joy_uid(0));
	u32 pad2_uid = Device::uid_to_index(g_conf.get_joy_uid(1));

    multitap_1_check->SetValue(g_conf.multitap[0]);
    multitap_2_check->SetValue(g_conf.multitap[1]);

	pad1_rumble_check->SetValue(g_conf.pad_options[0].forcefeedback);
	pad2_rumble_check->SetValue(g_conf.pad_options[1].forcefeedback);

	pad1_rumble_intensity->SetValue(g_conf.get_ff_intensity());
	pad1_joy_sensitivity->SetValue(g_conf.get_sensibility());

	pad2_rumble_intensity->SetValue(g_conf.get_ff_intensity());
	pad2_joy_sensitivity->SetValue(g_conf.get_sensibility());

	joy_choices.Clear();
	joy_choices.Add("None");
	for (const auto& j : device_manager->devices)
	{
		joy_choices.Add(j->GetName());
	}

	pad1_joy_choice->Set(joy_choices);
	pad2_joy_choice->Set(joy_choices);

	if (pad1_uid < (joy_choices.GetCount() - 1) && !pad1_joy_choice->IsEmpty())
		pad1_joy_choice->SetSelection(pad1_uid);

	if (pad2_uid < (joy_choices.GetCount() - 1) && !pad2_joy_choice->IsEmpty())
		pad2_joy_choice->SetSelection(pad2_uid);
}

void GeneralPanel::CallCheck(wxCommandEvent& /*event*/)
{
	g_conf.multitap[0] = multitap_1_check->GetValue();
	g_conf.multitap[1] = multitap_2_check->GetValue();
}

GeneralPanel::~GeneralPanel()
{

};

PADDialog::PADDialog()
	: wxDialog(nullptr, wxID_ANY, "Gamepad Settings", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX)
{
	auto* top_box = new wxBoxSizer(wxVERTICAL);

	// create a new Notebook
	m_tab_gamepad = new wxNotebook(this, wxID_ANY);
	general_panel = new GeneralPanel(m_tab_gamepad);
	m_tab_gamepad->AddPage(general_panel, "General");

    for (int port = 0; port < 2; port++)
    {
        for (int slot = 0; slot < 4; slot++)
        {
            wxString title;

            //if (config.padConfigs[port][slot].type == DisabledPad) continue;
			if (slot > 0) continue;
            if (!GetPadName(title, port, slot)) continue;

            m_gamepad_tabs.emplace_back(new GamepadPanel(m_tab_gamepad, port, slot));
            m_tab_gamepad->AddPage(m_gamepad_tabs.back(), title, false);
        }
    }
	
	top_box->Add(m_tab_gamepad);
	top_box->Add(CreateStdDialogButtonSizer(wxOK | wxAPPLY |wxCANCEL), wxSizerFlags().Right());

	Bind(wxEVT_BUTTON, &PADDialog::OnButtonClicked, this);
	Bind(wxEVT_SHOW, &PADDialog::OnWindowShown, this);
	SetSizerAndFit(top_box);
}

void PADDialog::InitDialog()
{
	EnumerateDevices();
	PADLoadConfig();

	general_panel->Update();
	general_panel->Populate();
}

void PADDialog::Update()
{
	for(auto& tab : m_gamepad_tabs)
	{
		tab->Update();
	}
}

/****************************************/
/*********** Events functions ***********/
/****************************************/

void PADDialog::OnWindowShown(wxShowEvent&)
{
	Update();
}

void PADDialog::OnButtonClicked(wxCommandEvent& event)
{
	// Affichage d'un message à chaque clic sur le bouton
	wxButton* bt_tmp = (wxButton*)event.GetEventObject(); // get the button object
	int actual_id = bt_tmp->GetId();

	switch(actual_id)
	{
		case wxID_OK:		 	 // If the button ID is equals to the Ok button ID
			PADSaveConfig();	 // Save the configuration
			EndModal(1);
			break;

		case wxID_APPLY:		 // If the button ID is equals to the Apply button ID
				PADSaveConfig(); // Save the configuration
			break;

		case wxID_CANCEL:		 // If the button ID is equals to the cancel button ID
				EndModal(0);
			break;
		
		default:
			break;
	}
}

// Main
void DisplayDialog()
{
	if (g_conf.ftw)
	{
		wxString info("The PAD GUI is provided to map the keyboard/mouse to the virtual PS2 pad.\n\n"
					  "Gamepads/Joysticks are plug and play. The active gamepad can be selected in the 'Gamepad Configuration' panel.\n\n");

		wxMessageDialog ftw(nullptr, info);
		ftw.ShowModal();

		g_conf.ftw = 0;
		PADSaveConfig();
	}

	PADDialog dialog;

	dialog.InitDialog();
	dialog.ShowModal();
}
