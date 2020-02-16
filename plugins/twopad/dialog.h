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

#pragma once

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/wrapsizer.h>

#include <array>

#include "ps2_pad.h"
#include "sdl_controller.h"
#include "keyboard.h"

const int DEFAULT_WIDTH = 400;
const int DEFAULT_HEIGHT = 430;

struct padControls
{
    wxStaticBoxSizer *box;
    wxChoice *controller_list;
    wxCheckBox *reversed_lx, *reversed_ly, *reversed_rx, *reversed_ry, *rumble;
};

struct keyControls
{
    wxStaticBoxSizer *box;
    std::array<wxButton*, MAX_KEYS> set_control, clear_control;
    std::array<wxStaticText*, MAX_KEYS> control_label;
};

struct dialog_pads
{
    padControls pad;
    keyControls key_pad;
    wxString name;
};

class configDialog : public wxDialog
{
    private:
        wxScrolledWindow *gamepad_page, *keyboard_page;
        std::array<dialog_pads, 2> controller;

        void addGamepad(int cpad);
        void addKeyboard(int cpad);

    public:
        configDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
	              const wxPoint & pos = wxDefaultPosition,
	              const wxSize & size = wxDefaultSize,
	              long style = wxDEFAULT_DIALOG_STYLE );
        ~configDialog();
        void Display();
        void setValues();
        void getValues();
        void configKey(wxCommandEvent &event);
};

extern configDialog *conf;
extern void initDialog();