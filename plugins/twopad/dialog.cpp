/*  TwoPAD - author: arcum42(@gmail.com)
 *  Copyright (C) 2019
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

configDialog *conf;

void initDialog()
{
    conf = new configDialog( nullptr, -1, _T("TwoPad"), wxDefaultPosition, /*wxDefaultSize*/wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT), wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
}

void configDialog::addGamepad(padControls &pad, const wxString controllerName)
{
    wxArrayString controllers;
    controllers.Add(_T("None"));
    for (auto pad : sdl_pad)
    {
        controllers.Add(pad->name);
    }

    pad.box = new wxStaticBoxSizer(wxVERTICAL, gamepad_page, controllerName);

    pad.controller_list = new wxChoice(
        gamepad_page, // Parent
        wxID_ANY,             // ID
        wxDefaultPosition,      // Position
        wxDefaultSize,        // Size
        controllers);
    pad.controller_list->SetStringSelection(_T("None"));

    auto *reverse_box = new wxBoxSizer(wxHORIZONTAL);
    pad.reversed_lx = new wxCheckBox(gamepad_page, wxID_ANY, "reverse LX");
    pad.reversed_ly = new wxCheckBox(gamepad_page, wxID_ANY, "reverse LY");
    reverse_box->Add(pad.reversed_lx);
    reverse_box->Add(pad.reversed_ly);

    pad.rumble = new wxCheckBox(gamepad_page, wxID_ANY, "Rumble");

    pad.box->Add(pad.controller_list);
    pad.box->Add(reverse_box);
    pad.box->Add(pad.rumble);
    pad.box->AddSpacer(20);
}

void configDialog::addKeyboard(keyControls &keys, const wxString controllerName)
{
    std::array<wxString, MAX_KEYS - 1> key_label = { "L2", "R2", "L1", "R1", "Triangle", "Circle", "Cross", "Square", "L3", "R3", "Start",
    "Pad Up", "Pad Right", "Pad Down", "Pad Left", 
    "Left Stick Up", "Left Stick Right", "Left Stick Down", "Left Stick Left", 
    "Right Stick Up", "Right Stick Right", "Right Stick Down", "Right Stick Left"};
    keys.box = new wxStaticBoxSizer(wxVERTICAL, keyboard_page, controllerName);

    int i = 0;
    for (auto label : key_label)
    {
        auto *box = new wxBoxSizer(wxHORIZONTAL);
        box->Add(new wxStaticText(keyboard_page, wxID_ANY, label), wxSizerFlags().Expand().Left());
        box->AddStretchSpacer();
        keys.set_control[i] = new wxButton(keyboard_page, 100 + i, _T("Set"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        keys.set_control[i]->Disable();
        box->Add(keys.set_control[i], wxSizerFlags().Expand().Right());
        keys.box->Add(box, wxSizerFlags().Expand().Center());
        i++;
    }
}

configDialog::configDialog( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position, const wxSize & size, long style )
: wxDialog( parent, id, title, position, size, style)
{
    auto *notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    gamepad_page = new wxScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    auto g_sizer = new wxBoxSizer(wxVERTICAL);

    addGamepad(pad1, _T("Controller 1"));
    addGamepad(pad2, _T("Controller 2"));
    
    g_sizer->Add(pad1.box, wxSizerFlags().Expand().Border(wxALL));
    g_sizer->Add(new wxStaticLine(gamepad_page, -1), wxSizerFlags().Center().Expand().Border(wxALL));
    g_sizer->Add(pad2.box, wxSizerFlags().Expand().Border(wxALL));

    notebook->AddPage(gamepad_page, "Controllers", true);
    gamepad_page->SetSizerAndFit(g_sizer);
    gamepad_page->SetVirtualSize(wxSize(2000,2000));
    gamepad_page->SetScrollRate(1, 1);

    keyboard_page = new wxScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    auto k_sizer = new wxBoxSizer(wxHORIZONTAL);
    addKeyboard(key_pad1, "Controller 1");
    k_sizer->Add(key_pad1.box);
    //k_sizer->AddStretchSpacer();
    addKeyboard(key_pad2, "Controller 2");
    k_sizer->Add(key_pad2.box);
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
    pad1.reversed_lx->SetValue(ps2_gamepad[0].reversed_lx);
    pad1.reversed_ly->SetValue(ps2_gamepad[0].reversed_ly);

    pad2.reversed_lx->SetValue(ps2_gamepad[1].reversed_lx);
    pad2.reversed_ly->SetValue(ps2_gamepad[1].reversed_ly);

    if (ps2_gamepad[0].controller_attached == false)
    {
        pad1.controller_list->SetSelection(0);
    }
    else
    {
       int idx = pad1.controller_list->FindString(ps2_gamepad[0].real->name);
       if (idx == wxNOT_FOUND) idx = 0;
       pad1.controller_list->SetSelection(idx);
    }

    if ((ps2_gamepad[0].real != nullptr) && (ps2_gamepad[0].real->rumble_supported))
    {
        pad1.rumble->SetValue(ps2_gamepad[0].real->rumble);
    }
    else
    {
        pad1.rumble->Disable();
    }

    if ((ps2_gamepad[1].real != nullptr) && (ps2_gamepad[1].real->rumble_supported))
    {
        pad2.rumble->SetValue(ps2_gamepad[1].real->rumble);
    }
    else
    {
        pad2.rumble->Disable();
    }
}

void configDialog::getValues()
{
    ps2_gamepad[0].reversed_lx = pad1.reversed_lx->GetValue();
    ps2_gamepad[0].reversed_ly = pad1.reversed_ly->GetValue();

    ps2_gamepad[1].reversed_lx = pad2.reversed_lx->GetValue();
    ps2_gamepad[1].reversed_ly = pad2.reversed_ly->GetValue();

    wxString pad1_name = pad1.controller_list->GetStringSelection();
    if (pad1_name == _T("None"))
    {
        ps2_gamepad[0].controller_attached = false;
        ps2_gamepad[0].real = nullptr;
    }
    else
    {
        for (auto pad : sdl_pad)
        {
            if (pad->name == pad1_name)
            {
                ps2_gamepad[0].controller_attached = true;
                ps2_gamepad[0].real = pad;
                ps2_gamepad[0].real->rumble = pad1.rumble->GetValue();
            }
        }
    }

    wxString pad2_name = pad2.controller_list->GetStringSelection();
    if (pad2_name == _T("None"))
    {
        ps2_gamepad[1].controller_attached = false;
        ps2_gamepad[1].real = nullptr;
    }
    else
    {
        for (auto pad : sdl_pad)
        {
            if (pad->name == pad2_name)
            {
                ps2_gamepad[1].controller_attached = true;
                ps2_gamepad[1].real = pad;
                ps2_gamepad[1].real->rumble = pad2.rumble->GetValue();
            }
        }
    }
}

void configDialog::Display()
{
    setValues();
    ShowModal();
    getValues();
}