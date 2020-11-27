/*  LilyPad - Pad plugin for PS2 Emulator
 *  Copyright (C) 2002-2020  PCSX2 Dev Team/ChickenLiver
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
#include "Config.h"

#include "wxDialog.h"

// Snagged from the Windows Config.cpp
//max len 24 wchar_t
const wchar_t *padTypes[] = {
    L"Unplugged",
    L"Dualshock 2",
    L"Guitar",
    L"Pop'n Music controller",
    L"PS1 Mouse",
    L"neGcon"};

// Returns 0 if pad doesn't exist due to mtap settings, as a convenience.
int GetPadName(wxString &string, unsigned int port, unsigned int slot)
{
    if (!slot && !config.multitap[port]) {
        string = wxString::Format(L"Pad %i", port + 1);

    } else {
        string = wxString::Format(L"Pad %i%c", port + 1, 'A' + slot);

        if (!config.multitap[port]) return 0;
    }
    return 1;
}

// Returns 0 if pad doesn't exist due to mtap settings, as a convenience.
int GetPadTypeName(wxString &string, unsigned int port, unsigned int slot, unsigned int padtype)
{
    if (!slot && !config.multitap[port]) {
        string = wxString::Format(L"Pad %i, %s", port + 1, padTypes[padtype]);

    } else {
        string = wxString::Format(L"Pad %i%c", port + 1, 'A' + slot, padTypes[padtype]);
        if (!config.multitap[port]) return 0;
    }
    return 1;
}

GeneralTab::GeneralTab(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	auto* tab_box = new wxBoxSizer(wxVERTICAL);

	auto* temp_text = new wxStaticText(this, wxID_ANY, "This is a placeholder for general settings.");
	tab_box->Add(temp_text);

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHECKBOX, &GeneralTab::CallUpdate, this);
}

void GeneralTab::CallUpdate(wxCommandEvent& /*event*/)
{
	Update();
}

void GeneralTab::Update()
{

}

PadTab::PadTab(wxWindow* parent, unsigned int port, unsigned int slot)
	: wxPanel(parent, wxID_ANY)
{
	auto* tab_box = new wxBoxSizer(wxVERTICAL);
    wxString title;
    GetPadName(title, port, slot);

	auto* temp_text = new wxStaticText(this, wxID_ANY, "This is a placeholder for " + title + " settings.");
	tab_box->Add(temp_text);

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHECKBOX, &PadTab::CallUpdate, this);
}

void PadTab::CallUpdate(wxCommandEvent& /*event*/)
{
	Update();
}

void PadTab::Update()
{

}

Dialog::Dialog()
	: wxDialog(nullptr, wxID_ANY, "Controller Config", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX)
{
	auto* padding = new wxBoxSizer(wxVERTICAL);
	m_top_box = new wxBoxSizer(wxVERTICAL);

    auto* book = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    auto* m_general_panel = new GeneralTab(book);
    book->AddPage(m_general_panel, "General", true);

    for (int port = 0; port < 2; port++)
    {
        for (int slot = 0; slot < 4; slot++)
        {
            wxString title;

            // Uncomment when it actually puts *any* pads as not disabled.
            //if (config.padConfigs[port][slot].type == DisabledPad) continue;
            if (!GetPadName(title, port, slot)) continue;

            auto* m_pad_panel = new PadTab(book, port, slot);
            book->AddPage(m_pad_panel, title, true);
        }
    }

    m_top_box->Add(book, wxSizerFlags().Centre().Expand());
    padding->Add(m_top_box, wxSizerFlags().Centre().Expand().Border(wxALL, 5));

	m_top_box->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Right());

	SetSizerAndFit(padding);
	Bind(wxEVT_CHECKBOX, &Dialog::CallUpdate, this);
}

Dialog::~Dialog()
{
}

void Dialog::CallUpdate(wxCommandEvent& /*event*/)
{
	Update();
}

void Dialog::Update()
{

}
