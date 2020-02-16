/*  TwoPAD - author: arcum42(@gmail.com)
 *  Copyright (C) 2019-2020
 *
 *  Based on ZeroPAD, author zerofrog@gmail.com
 *  And OnePAD, author arcum42, gregory, PCSX2 team
 *  Copyright (C) 2006-2007
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "dialog.h"
#include "settings.h"

#include "linux/keyboard_x11.h"

configDialog *conf;

void initDialog()
{
    conf = new configDialog( nullptr, -1, _T("TwoPad"), wxDefaultPosition, wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT), wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
}

void configDialog::addGamepad(int cpad)
{
    wxArrayString controllers;
    controllers.Add(_T("None"));

    for (auto pad : sdl_pad)
    {
        controllers.Add(pad->name);
    }

    controller[cpad].pad.box = new wxStaticBoxSizer(wxVERTICAL, gamepad_page, controller[cpad].name);

    controller[cpad].pad.controller_list = new wxChoice(
        gamepad_page, // Parent
        wxID_ANY,             // ID
        wxDefaultPosition,      // Position
        wxDefaultSize,        // Size
        controllers);
    controller[cpad].pad.controller_list->SetStringSelection(_T("None"));

    auto *reverse_box = new wxBoxSizer(wxHORIZONTAL);
    controller[cpad].pad.reversed_lx = new wxCheckBox(gamepad_page, wxID_ANY, "reverse LX");
    controller[cpad].pad.reversed_ly = new wxCheckBox(gamepad_page, wxID_ANY, "reverse LY");
    reverse_box->Add(controller[cpad].pad.reversed_lx);
    reverse_box->Add(controller[cpad].pad.reversed_ly);

    controller[cpad].pad.rumble = new wxCheckBox(gamepad_page, wxID_ANY, "Rumble");

    controller[cpad].pad.box->Add(controller[cpad].pad.controller_list);
    controller[cpad].pad.box->Add(reverse_box);
    controller[cpad].pad.box->Add(controller[cpad].pad.rumble);
    controller[cpad].pad.box->AddSpacer(20);
}

void configDialog::addKeyboard(int cpad)
{
    controller[cpad].key_pad.box = new wxStaticBoxSizer(wxVERTICAL, keyboard_page, controller[cpad].name);

    for (int c = 0; c < MAX_KEYS; c++)
    {
        auto *box = new wxBoxSizer(wxHORIZONTAL);
        auto label_text = gamepad_names[c] + " = '" + keys->control_to_string(cpad, c) + "'";

        controller[cpad].key_pad.set_control[c] = new wxButton(keyboard_page, 100 + c, _T("Set"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        controller[cpad].key_pad.set_control[c]->Disable();
        box->Add(controller[cpad].key_pad.set_control[c], wxSizerFlags().Expand().Right());

        box->Add(new wxStaticText(keyboard_page, wxID_ANY, label_text), wxSizerFlags().Expand().Left());
        box->AddStretchSpacer();
        controller[cpad].key_pad.box->Add(box, wxSizerFlags().Expand().Center());
    }
}

configDialog::configDialog( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position, const wxSize & size, long style )
: wxDialog( parent, id, title, position, size, style)
{
    controller[0].name = _T("Controller 1");
    controller[1].name = _T("Controller 2");

    auto *notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    gamepad_page = new wxScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    auto g_sizer = new wxBoxSizer(wxVERTICAL);

    addGamepad(0);
    g_sizer->Add(controller[0].pad.box, wxSizerFlags().Expand().Border(wxALL));

    g_sizer->Add(new wxStaticLine(gamepad_page, -1), wxSizerFlags().Center().Expand().Border(wxALL));

    addGamepad(1);
    g_sizer->Add(controller[1].pad.box, wxSizerFlags().Expand().Border(wxALL));

    notebook->AddPage(gamepad_page, "Controllers", true);
    gamepad_page->SetSizerAndFit(g_sizer);
    gamepad_page->SetVirtualSize(wxSize(2000,2000));
    gamepad_page->SetScrollRate(1, 1);

    keyboard_page = new wxScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    auto k_sizer = new wxBoxSizer(wxHORIZONTAL);

    addKeyboard(0);
    k_sizer->Add(controller[0].key_pad.box);

    addKeyboard(1);
    k_sizer->Add(controller[1].key_pad.box);

    keyboard_page->SetSizerAndFit(k_sizer);
    keyboard_page->SetVirtualSize(wxSize(2000,2000));
    keyboard_page->SetScrollRate(1, 1);

    notebook->AddPage(keyboard_page, "Keyboard", false);
}

configDialog::~configDialog()
{
     Destroy();
}

void configDialog::setValues()
{
    controller[0].pad.reversed_lx->SetValue(ps2_gamepad[0]->reversed_lx);
    controller[0].pad.reversed_ly->SetValue(ps2_gamepad[0]->reversed_ly);

    controller[1].pad.reversed_lx->SetValue(ps2_gamepad[1]->reversed_lx);
    controller[1].pad.reversed_ly->SetValue(ps2_gamepad[1]->reversed_ly);

    if (ps2_gamepad[0]->controller_attached == false)
    {
        controller[0].pad.controller_list->SetSelection(0);
    }
    else
    {
       int idx = controller[0].pad.controller_list->FindString(ps2_gamepad[0]->real->name);
       if (idx == wxNOT_FOUND) idx = 0;
       controller[0].pad.controller_list->SetSelection(idx);
    }

    if ((ps2_gamepad[0]->real != nullptr) && (ps2_gamepad[0]->real->rumble_supported))
    {
        controller[0].pad.rumble->SetValue(ps2_gamepad[0]->real->rumble);
    }
    else
    {
        controller[0].pad.rumble->Disable();
    }

    if ((ps2_gamepad[1]->real != nullptr) && (ps2_gamepad[1]->real->rumble_supported))
    {
        controller[1].pad.rumble->SetValue(ps2_gamepad[1]->real->rumble);
    }
    else
    {
        controller[1].pad.rumble->Disable();
    }
}

void configDialog::getValues()
{
    ps2_gamepad[0]->reversed_lx = controller[0].pad.reversed_lx->GetValue();
    ps2_gamepad[0]->reversed_ly = controller[0].pad.reversed_ly->GetValue();

    ps2_gamepad[1]->reversed_lx = controller[1].pad.reversed_lx->GetValue();
    ps2_gamepad[1]->reversed_ly = controller[1].pad.reversed_ly->GetValue();

    wxString pad1_name = controller[0].pad.controller_list->GetStringSelection();
    if (pad1_name == _T("None"))
    {
        ps2_gamepad[0]->controller_attached = false;
        ps2_gamepad[0]->real = nullptr;
    }
    else
    {
        for (auto pad : sdl_pad)
        {
            if (pad->name == pad1_name)
            {
                ps2_gamepad[0]->controller_attached = true;
                ps2_gamepad[0]->real = pad;
                ps2_gamepad[0]->real->rumble = controller[0].pad.rumble->GetValue();
            }
        }
    }

    wxString pad2_name = controller[1].pad.controller_list->GetStringSelection();
    if (pad2_name == _T("None"))
    {
        ps2_gamepad[1]->controller_attached = false;
        ps2_gamepad[1]->real = nullptr;
    }
    else
    {
        for (auto pad : sdl_pad)
        {
            if (pad->name == pad2_name)
            {
                ps2_gamepad[1]->controller_attached = true;
                ps2_gamepad[1]->real = pad;
                ps2_gamepad[1]->real->rumble = controller[1].pad.rumble->GetValue();
            }
        }
    }
}

void configDialog::Display()
{
    load_config();
    setValues();
    ShowModal();
    getValues();
    save_config();
}