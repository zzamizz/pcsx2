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

	for(u32 i = 0; i < 2; i++)
	{
    	auto* pad_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::Format("Pad %d", i + 1));

		pad[i].multitap_check = new wxCheckBox(this, wxID_ANY, wxString::Format("Port %d Multitap", i + 1));
		pad[i].joy_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(250,-1), joy_choices);
		pad[i].rumble_check = new wxCheckBox(this, wxID_ANY, _T("&Enable rumble"));

		auto* rumble_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Rumble intensity"));
		pad[i].rumble_intensity = new wxSlider(this, wxID_ANY, 0, 0, 0x7FFF, wxDefaultPosition, wxSize(250,-1),
											wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
		rumble_box->Add(pad[i].rumble_intensity);

		auto* joy_box = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Joystick sensitivity"));
		pad[i].joy_sensitivity = new wxSlider(this, wxID_ANY, 0, 0, 200, wxDefaultPosition, wxSize(250,-1),
												wxSL_HORIZONTAL | wxSL_LABELS | wxSL_BOTTOM);
		joy_box->Add(pad[i].joy_sensitivity);

		pad_box->Add(pad[i].multitap_check);
		pad_box->Add(pad[i].joy_choice);
		pad_box->Add(pad[i].rumble_check);
		pad_box->Add(rumble_box);
		pad_box->Add(joy_box);

		tab_box->Add(pad_box, wxSizerFlags().Border(wxALL, 10));
	}

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHECKBOX, &GeneralPanel::CallCheck, this);

	Populate();
};

void GeneralPanel::Populate()
{
	joy_choices.Clear();
	joy_choices.Add("None");
	for (const auto& j : device_manager->devices)
	{
		joy_choices.Add(j->GetName());
	}

	for(u32 i = 0; i < 2; i++)
	{
		u32 pad_uid = Device::uid_to_index(g_conf.get_joy_uid(i));
		pad[i].multitap_check->SetValue(g_conf.multitap[i]);
		pad[i].rumble_check->SetValue(g_conf.options[i].forcefeedback);
		pad[i].rumble_intensity->SetValue(g_conf.get_ff_intensity());
		pad[i].joy_sensitivity->SetValue(g_conf.get_sensibility());

		pad[i].joy_choice->Set(joy_choices);

		if (pad_uid < (joy_choices.GetCount() - 1) && !pad[i].joy_choice->IsEmpty())
			pad[i].joy_choice->SetSelection(pad_uid);
	}
}

void GeneralPanel::CallCheck(wxCommandEvent& /*event*/)
{
	g_conf.multitap[0] = pad[1].multitap_check->GetValue();
	g_conf.multitap[1] = pad[1].multitap_check->GetValue();
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
