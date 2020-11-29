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

#pragma once

#include <wx/wx.h>
#include <wx/arrstr.h>

#include <wx/notebook.h>
#include <wx/dataview.h>

#include <wx/textctrl.h>

#include <vector>

class NoteBook : public wxNotebook
{
private:
public:
    NoteBook(wxWindow* parent);
    ~NoteBook();
    void RecreatePadTabs();
};

class Dialog : public wxDialog
{
private:
	wxBoxSizer* m_top_box;
    NoteBook* book;

public:
	Dialog();
	~Dialog();
	void Update();
	void CallUpdate(wxCommandEvent& event);
};

struct pad_location
{
    unsigned int port;
    unsigned int slot;
};

class GeneralTab : public wxPanel
{
private:
    wxCheckBox *multitap_1_check, *multitap_2_check, *multiple_bindings_check;
    wxChoice *choice;
    wxDataViewListCtrl *pad_list;
    std::vector<pad_location> loc;
    NoteBook *book;

public:
	GeneralTab(NoteBook* parent);
	void RefreshList();
	void CallRefreshList(wxCommandEvent& event);
	void UpdateType();
	void CallUpdateType(wxCommandEvent& event);
	void UpdateCheck();
	void CallCheck(wxCommandEvent& event);
};

class PadTab : public wxPanel
{
private:
    wxDataViewListCtrl *pad_list;
	wxTextCtrl *status_bar;

public:
	PadTab(NoteBook* parent, unsigned int port, unsigned int slot);
	void Update();
	void CallUpdate(wxCommandEvent& event);
    void Populate(int port, int slot, int padtype);
    int ListBoundCommand(int port, int slot, Device *dev, Binding *b);
};

enum
{
    wxBTN_PAD_ID_L1 = wxID_HIGHEST + 1,
    wxBTN_PAD_ID_R1 = wxID_HIGHEST + 2,
    wxBTN_PAD_ID_L2 = wxID_HIGHEST + 3,
    wxBTN_PAD_ID_R2 = wxID_HIGHEST + 4,

    wxBTN_PAD_ID_START = wxID_HIGHEST + 5,
    wxBTN_PAD_ID_SELECT = wxID_HIGHEST + 6,
    wxBTN_PAD_ID_ANALOG = wxID_HIGHEST + 7,
    wxBTN_PAD_ID_MOUSE = wxID_HIGHEST + 8,

    wxBTN_PAD_ID_D_PAD_U = wxID_HIGHEST + 9,
    wxBTN_PAD_ID_D_PAD_L = wxID_HIGHEST + 10,
    wxBTN_PAD_ID_D_PAD_R = wxID_HIGHEST + 11,
    wxBTN_PAD_ID_D_PAD_D = wxID_HIGHEST + 12,

    wxBTN_PAD_ID_TRIANGLE = wxID_HIGHEST + 13,
    wxBTN_PAD_ID_SQUARE = wxID_HIGHEST + 14,
    wxBTN_PAD_ID_CIRCLE = wxID_HIGHEST + 15,
    wxBTN_PAD_ID_CROSS = wxID_HIGHEST + 16,

    wxBTN_PAD_ID_L_STICK_U = wxID_HIGHEST + 17,
    wxBTN_PAD_ID_L_STICK_L = wxID_HIGHEST + 18,
    wxBTN_PAD_ID_L_STICK_R = wxID_HIGHEST + 19,
    wxBTN_PAD_ID_L_STICK_D = wxID_HIGHEST + 20,

    wxBTN_PAD_ID_R_STICK_U = wxID_HIGHEST + 21,
    wxBTN_PAD_ID_R_STICK_L = wxID_HIGHEST + 22,
    wxBTN_PAD_ID_R_STICK_R = wxID_HIGHEST + 23,
    wxBTN_PAD_ID_R_STICK_D = wxID_HIGHEST + 24
};