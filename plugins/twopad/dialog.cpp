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
    conf = new configDialog( nullptr, -1, _T("TwoPad"), wxDefaultPosition, wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT), wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
}

void configDialog::addGamepad(padControls &pad, const wxString controllerName)
{
    wxArrayString controllers;
    controllers.Add(_T("None"));
    for (auto pad : sdl_pad)
    {
        controllers.Add(pad->name);
    }

    pad.Box = new wxStaticBoxSizer(wxVERTICAL, panel, controllerName);

    pad.Ctl = new wxChoice(
        panel, // Parent
        wxID_ANY,             // ID
        wxDefaultPosition,      // Position
        wxDefaultSize,        // Size
        controllers);
    pad.Ctl->SetStringSelection(_T("None"));

    auto *rev = new wxBoxSizer(wxHORIZONTAL);
    pad.ReversedLX = new wxCheckBox(panel, wxID_ANY, "reverse LX");
    pad.ReversedLY = new wxCheckBox(panel, wxID_ANY, "reverse LY");
    rev->Add(pad.ReversedLX);
    rev->Add(pad.ReversedLY);

    pad.Rumble = new wxCheckBox(panel, wxID_ANY, "Rumble");
    // Rumble isn't implemented yet.
    pad.Rumble->Disable();

    pad.Box->Add(pad.Ctl);
    pad.Box->Add(rev);
    pad.Box->Add(pad.Rumble);
}

configDialog::configDialog( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position, const wxSize & size, long style )
: wxDialog( parent, id, title, position, size, style)
{
    panel = new wxPanel(this, -1);
    auto sizer = new wxBoxSizer(wxVERTICAL);

    addGamepad(pad1, _T("Controller 1"));
    addGamepad(pad2, _T("Controller 2"));
    
    sizer->Add(pad1.Box, 1, wxEXPAND | wxALL, 5);
    sizer->Add(
        new wxStaticLine(panel, -1),
        wxSizerFlags(1).Center().Expand().Border(wxALL, 3));
    sizer->Add(pad2.Box, 3, wxEXPAND | wxALL, 5);

    panel->SetSizerAndFit(sizer);
}

configDialog::~configDialog()
{
     Destroy();
}

void configDialog::setValues()
{
    pad1.ReversedLX->SetValue(ps2_gamepad[0].reversed_lx);
    pad1.ReversedLY->SetValue(ps2_gamepad[0].reversed_ly);

    pad2.ReversedLX->SetValue(ps2_gamepad[1].reversed_lx);
    pad2.ReversedLY->SetValue(ps2_gamepad[1].reversed_ly);

    if (ps2_gamepad[0].controller_attached == false)
    {
        pad1.Ctl->SetSelection(0);
    }
    else
    {
       int idx = pad1.Ctl->FindString(ps2_gamepad[0].real->name);
       if (idx == wxNOT_FOUND) idx = 0;
       pad1.Ctl->SetSelection(idx);
    }
}

void configDialog::getValues()
{
    ps2_gamepad[0].reversed_lx = pad1.ReversedLX->GetValue();
    ps2_gamepad[0].reversed_ly = pad1.ReversedLY->GetValue();

    ps2_gamepad[1].reversed_lx = pad2.ReversedLX->GetValue();
    ps2_gamepad[1].reversed_ly = pad2.ReversedLY->GetValue();

    wxString pad1_name = pad1.Ctl->GetStringSelection();
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
            }
        }
    }

    wxString pad2_name = pad2.Ctl->GetStringSelection();
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