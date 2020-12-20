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
	auto* tab_box = new wxBoxSizer(wxHORIZONTAL);
    auto* pad_box = new wxStaticBoxSizer(wxHORIZONTAL, this, "Pads");
    auto* pad_options = new wxBoxSizer(wxVERTICAL);

    multitap_1_check = new wxCheckBox(this, wxID_ANY, "Port 1 Multitap");
    multitap_2_check = new wxCheckBox(this, wxID_ANY, "Port 2 Multitap");
    multiple_bindings_check = new wxCheckBox(this, wxID_ANY, "Multiple Bindings");

    //multiple_bindings_check->SetValue(config.multipleBinding);
    multitap_1_check->SetValue(g_conf.multitap[0]);
    multitap_2_check->SetValue(g_conf.multitap[1]);

    pad_options->Add(multitap_1_check);
    pad_options->Add(multitap_2_check);
    pad_options->Add(multiple_bindings_check);

    pad_box->Add(pad_options);

    pad_list = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(350,100));
    pad_list->AppendTextColumn("Pad", wxDATAVIEW_CELL_INERT, 60);
    pad_list->AppendTextColumn("Type", wxDATAVIEW_CELL_INERT, 186);
    pad_list->AppendTextColumn("Bindings", wxDATAVIEW_CELL_INERT, 40);

    pad_box->Add(pad_list);

    wxArrayString why;
    for(auto str : padTypes)
    {
        why.Add(str);
    }

    choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, why);
    pad_box->Add(choice);

    RefreshList();
    
    tab_box->Add(pad_box);

	SetSizerAndFit(tab_box);
	//Bind(wxEVT_CHOICE, &GeneralTab::CallRefreshList, this);
	//Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &GeneralTab::CallUpdateType, this);
	Bind(wxEVT_CHECKBOX, &GeneralPanel::CallCheck, this);
};

void GeneralPanel::CallCheck(wxCommandEvent& /*event*/)
{
	g_conf.multitap[0] = multitap_1_check->GetValue();
	g_conf.multitap[1] = multitap_2_check->GetValue();
	//g_conf.multipleBinding = multiple_bindings_check->GetValue();

}
// Updates the list with the current information.
void GeneralPanel::RefreshList()
{
    //int list_selection = pad_list->GetSelectedRow();
    //int choice_selection = choice->GetSelection();

    //if (choice_selection >= 0)
    //{
    //    config.padConfigs[loc[list_selection].port][loc[list_selection].slot].type = (PadType)choice_selection; 
    //}

    pad_list->DeleteAllItems();
    //loc.clear();
    
    for (unsigned int port = 0; port < 2; port++)
    {
        for (unsigned int slot = 0; slot < 4; slot++)
        {
            wxString title;
            wxVector<wxVariant> data;

            if (!GetPadName(title, port, slot)) continue;

            data.push_back(wxVariant(title));
			data.push_back(wxVariant(wxString("Placeholder")));
            data.push_back(wxVariant(wxString::Format("%d", 0)));
            //data.push_back(wxVariant(padTypes[config.padConfigs[port][slot].type]));
            //data.push_back(wxVariant(wxString::Format("%d", CountBindings(port, slot))));
            pad_list->AppendItem(data);
            //loc.push_back({port, slot});
        }
    }
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
	EnumerateDevices();			// activate gamepads
	PADLoadConfig();                        // Load configuration from the ini file
	general_panel->Update();                           // Set label and fit simulated key array
}

void PADDialog::Update()
{
	general_panel->RefreshList();
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
