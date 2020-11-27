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

unsigned int CountBindings(unsigned int port, unsigned int slot)
{
    unsigned int count = 0;
    const int padtype = config.padConfigs[port][slot].type;

    for (int i = 0; i < dm->numDevices; i++)
    {
        Device *dev = dm->devices[i];
        if (!dev->enabled) continue;

        count += dev->pads[port][slot][padtype].numBindings + dev->pads[port][slot][padtype].numFFBindings;
    }
    return count;
}

int GetItemIndex(int port, int slot, Device *dev, ForceFeedbackBinding *binding)
{
    int count = 0;
    int padtype = config.padConfigs[port][slot].type;
    int selectedDevice = config.deviceSelect[port][slot];
    for (int i = 0; i < dm->numDevices; i++) {
        Device *dev2 = dm->devices[i];
        if (!dev2->enabled || selectedDevice >= 0 && dm->devices[selectedDevice] != dev2)
            continue;
        if (dev2 != dev) {
            count += dev2->pads[port][slot][padtype].numBindings + dev2->pads[port][slot][padtype].numFFBindings;
            continue;
        }
        return count += dev2->pads[port][slot][padtype].numBindings + (binding - dev2->pads[port][slot][padtype].ffBindings);
    }
    return -1;
}

int GetItemIndex(int port, int slot, Device *dev, Binding *binding)
{
    int count = 0;
    int padtype = config.padConfigs[port][slot].type;
    int selectedDevice = config.deviceSelect[port][slot];
    for (int i = 0; i < dm->numDevices; i++) {
        Device *dev2 = dm->devices[i];
        if (!dev2->enabled || selectedDevice >= 0 && dm->devices[selectedDevice] != dev2)
            continue;
        if (dev2 != dev) {
            count += dev2->pads[port][slot][padtype].numBindings + dev2->pads[port][slot][padtype].numFFBindings;
            continue;
        }
        return count += binding - dev->pads[port][slot][padtype].bindings;
    }
    return -1;
}

const wchar_t *GetCommandStringW(u8 command, int port, int slot)
{
    int padtype = config.padConfigs[port][slot].type;
    static wchar_t temp[34];

    if (command >= 0x20 && command <= 0x27)
    {
        //if (padtype == GuitarPad && (command == 0x20 || command == 0x22))
        //{
            //HWND hWnd = GetDlgItem(hWnds[port][slot][padtype], 0x10F0 + command);
            //int res = GetWindowTextW(hWnd, temp, 20);
            //if ((unsigned int)res - 1 <= 18)
            //    return temp;
        //}
        static const wchar_t *stick[2] = {L"L-Stick", L"R-Stick"};
        static const wchar_t *dir[] = {
            L"Up", L"Right",
            L"Down", L"Left"};
        wsprintfW(temp, L"%s %s", padtype == neGconPad ? L"Rotate" : stick[(command - 0x20) / 4], dir[command & 3]);
        return temp;
    }
    /* Get text from the buttons. */
    if (command >= 0x0F && command <= 0x2D)
    {
        //HWND hWnd = GetDlgItem(hWnds[port][slot][padtype], 0x10F0 + command);
        if /*(!hWnd ||*/ ((padtype == Dualshock2Pad || padtype == neGconPad) && command >= 0x14 && command <= 0x17) //)
        {
            const wchar_t *strings[] =
            {
                L"Mouse",          // 0x0F (15)
                L"Select",         // 0x10 (16)
                L"L3",             // 0x11 (17)
                L"R3",             // 0x12 (18)
                L"Start",          // 0x13 (19)
                L"D-Pad Up",       // 0x14 (20)
                L"D-Pad Right",    // 0x15 (21)
                L"D-Pad Down",     // 0x16 (22)
                L"D-Pad Left",     // 0x17 (23)
                L"L2",             // 0x18 (24)
                L"R2",             // 0x19 (25)
                L"L1",             // 0x1A (26)
                L"R1",             // 0x1B (27)
                L"Triangle",       // 0x1C (28)
                L"Circle",         // 0x1D (29)
                L"Square",         // 0x1E (30)
                L"Cross",          // 0x1F (31)
                L"L-Stick Up",     // 0x20 (32)
                L"L-Stick Right",  // 0x21 (33)
                L"L-Stick Down",   // 0x22 (34)
                L"L-Stick Left",   // 0x23 (35)
                L"R-Stick Up",     // 0x24 (36)
                L"R-Stick Right",  // 0x25 (37)
                L"R-Stick Down",   // 0x26 (38)
                L"R-Stick Left",   // 0x27 (39)
                L"Analog",         // 0x28 (40)
                L"Excluded Input", // 0x29 (41)
                L"Lock Buttons",   // 0x2A (42)
                L"Lock Input",     // 0x2B (43)
                L"Lock Direction", // 0x2C (44)
                L"Turbo",          // 0x2D (45)
            };
            return strings[command - 0xF];
        }
        //int res = GetWindowTextW(hWnd, temp, 20);
        //if ((unsigned int)res - 1 <= 18)
        //    return temp;
    }
    return L"";
}

GeneralTab::GeneralTab(NoteBook* parent)
	: wxPanel(parent, wxID_ANY)
{
    book = parent;
	auto* tab_box = new wxBoxSizer(wxHORIZONTAL);
    auto* pad_box = new wxStaticBoxSizer(wxHORIZONTAL, this, "Pads");
    auto* pad_options = new wxBoxSizer(wxVERTICAL);

    multitap_1_check = new wxCheckBox(this, wxID_ANY, "Port 1 Multitap");
    multitap_2_check = new wxCheckBox(this, wxID_ANY, "Port 2 Multitap");
    multiple_bindings_check = new wxCheckBox(this, wxID_ANY, "Multiple Bindings");

    multiple_bindings_check->SetValue(config.multipleBinding);
    multitap_1_check->SetValue(config.bools[7]);
    multitap_2_check->SetValue(config.bools[8]);

    pad_options->Add(multitap_1_check);
    pad_options->Add(multitap_2_check);
    pad_options->Add(multiple_bindings_check);

    pad_box->Add(pad_options);

    pad_list = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(350,100));
    pad_list->AppendTextColumn("Pad", wxDATAVIEW_CELL_INERT, 60);
    pad_list->AppendTextColumn("Type", wxDATAVIEW_CELL_INERT, 186);
    pad_list->AppendTextColumn("Bindings", wxDATAVIEW_CELL_INERT, 40);

    pad_box->Add(pad_list, wxSizerFlags().Expand());

    wxArrayString why;
    for(auto str : padTypes)
    {
        why.Add(str);
    }
    choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, why);
    pad_box->Add(choice);

    RefreshList();
    
    tab_box->Add(pad_box, wxSizerFlags().Centre().Expand());

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHOICE, &GeneralTab::CallRefreshList, this);
	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &GeneralTab::CallUpdateType, this);
	Bind(wxEVT_CHECKBOX, &GeneralTab::CallCheck, this);
}

void GeneralTab::CallRefreshList(wxCommandEvent& /*event*/)
{
	RefreshList();
}

void GeneralTab::CallUpdateType(wxCommandEvent& /*event*/)
{
	UpdateType();
}

void GeneralTab::CallCheck(wxCommandEvent& /*event*/)
{
	UpdateCheck();
}

// Updates the list with the current information.
void GeneralTab::RefreshList()
{
    int list_selection = pad_list->GetSelectedRow();
    int choice_selection = choice->GetSelection();

    if (choice_selection >= 0)
    {
        config.padConfigs[loc[list_selection].port][loc[list_selection].slot].type = (PadType)choice_selection; 
    }

    pad_list->DeleteAllItems();
    loc.clear();
    
    for (unsigned int port = 0; port < 2; port++)
    {
        for (unsigned int slot = 0; slot < 4; slot++)
        {
            wxString title;
            wxVector<wxVariant> data;

            if (!GetPadName(title, port, slot)) continue;

            data.push_back(wxVariant(title));
            data.push_back(wxVariant(padTypes[config.padConfigs[port][slot].type]));
            data.push_back(wxVariant(wxString::Format("%d", CountBindings(port, slot))));
            pad_list->AppendItem(data);
            loc.push_back({port, slot});
        }
    }
}

// Updates the selected item with the type chosen.
void GeneralTab::UpdateType()
{
    int selection = pad_list->GetSelectedRow();

    if (selection >= 0)
    {
        unsigned int port = 0;
        unsigned int slot = 0;

        port = loc[selection].port;
        slot = loc[selection].slot;
        choice->SetSelection(config.padConfigs[port][slot].type);
    }
}

void GeneralTab::UpdateCheck()
{
    config.multipleBinding = multiple_bindings_check->GetValue();
    config.bools[7] = multitap_1_check->GetValue();
    config.bools[8] = multitap_2_check->GetValue();
    RefreshList();
    book->RecreatePadTabs();
}

PadTab::PadTab(NoteBook* parent, unsigned int port, unsigned int slot)
	: wxPanel(parent, wxID_ANY)
{
	auto* tab_box = new wxBoxSizer(wxVERTICAL);
    wxString title;
    GetPadName(title, port, slot);

	auto* temp_text = new wxStaticText(this, wxID_ANY, "This is a placeholder for " + title + " settings.");
	tab_box->Add(temp_text);

    pad_list = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(360,200));
    pad_list->AppendTextColumn("Device", wxDATAVIEW_CELL_INERT, 120);
    pad_list->AppendTextColumn("PC", wxDATAVIEW_CELL_INERT, 120);
    pad_list->AppendTextColumn("PS2", wxDATAVIEW_CELL_INERT, 120);

    Update();

	SetSizerAndFit(tab_box);
	Bind(wxEVT_CHECKBOX, &PadTab::CallUpdate, this);
}

void PadTab::CallUpdate(wxCommandEvent& /*event*/)
{
	Update();
}

// Doesn't check if already displayed.
int PadTab::ListBoundCommand(int port, int slot, Device *dev, Binding *b)
{
    //int padtype = config.padConfigs[port][slot].type;
    //if (!hWnds[port][slot][padtype])
    //    return -1;
    //HWND hWndList = GetDlgItem(hWnds[port][slot][padtype], IDC_BINDINGS_LIST);
    int index = -1;
    //if (hWndList) 
    //{
        index = GetItemIndex(port, slot, dev, b);
        if (index >= 0)
        {
            wxVector<wxVariant> data;

            data.push_back(wxVariant(dev->displayName));
            data.push_back(wxVariant(dev->GetVirtualControlName(&dev->virtualControls[b->controlIndex])));
            data.push_back(GetCommandStringW(b->command, port, slot));
        }
    //}
    return index;
}

void PadTab::Populate(int port, int slot, int padtype)
{
    int multipleBinding = config.multipleBinding;
    config.multipleBinding = 1;
    int selectedDevice = config.deviceSelect[port][slot];

    for (int j = 0; j < dm->numDevices; j++)
    {
        Device *dev = dm->devices[j];

        if (!dev->enabled || selectedDevice >= 0 && dm->devices[selectedDevice] != dev)
            continue;

        for (int i = 0; i < dev->pads[port][slot][padtype].numBindings; i++)
        {
            ListBoundCommand(port, slot, dev, dev->pads[port][slot][padtype].bindings + i);
        }

        /*for (int i = 0; i < dev->pads[port][slot][padtype].numFFBindings; i++)
        {
            ListBoundEffect(port, slot, dev, dev->pads[port][slot][padtype].ffBindings + i);
        }*/
    }
    config.multipleBinding = multipleBinding;

}

void PadTab::Update()
{
    for (int j = 0; j < numPadTypes; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            Populate(i & 1, i >> 1, j);
        }
    }
    //for (unsigned int port = 0; port < 2; port++)
    //{
    //    for (unsigned int slot = 0; slot < 4; slot++)
    //    {
            /*wxString title;
            wxVector<wxVariant> data;

            if (!GetPadName(title, port, slot)) continue;

            data.push_back(wxVariant(title));
            data.push_back(wxVariant(padTypes[config.padConfigs[port][slot].type]));
            data.push_back(wxVariant(wxString::Format("%d", CountBindings(port, slot))));
            pad_list->AppendItem(data);
            loc.push_back({port, slot});*/
    //    }
    //}

/*     if (populate) {
        for (int j = 0; j < numPadTypes; j++) {
            for (int i = 0; i < 8; i++) {
                Populate(i & 1, i >> 1, j);
            }
        }
    } */
    /* -- Populate
    int multipleBinding = config.multipleBinding;
    config.multipleBinding = 1;
    int selectedDevice = config.deviceSelect[port][slot];
    for (int j = 0; j < dm->numDevices; j++) {
        Device *dev = dm->devices[j];
        if (!dev->enabled || selectedDevice >= 0 && dm->devices[selectedDevice] != dev)
            continue;
        for (int i = 0; i < dev->pads[port][slot][padtype].numBindings; i++) {
            ListBoundCommand(port, slot, dev, dev->pads[port][slot][padtype].bindings + i);
        }
        for (int i = 0; i < dev->pads[port][slot][padtype].numFFBindings; i++) {
            ListBoundEffect(port, slot, dev, dev->pads[port][slot][padtype].ffBindings + i);
        }
    }
    config.multipleBinding = multipleBinding; */


}

NoteBook::NoteBook(wxWindow* parent) : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    auto* m_general_panel = new GeneralTab(this);
    AddPage(m_general_panel, "General", true);

    RecreatePadTabs();
}

void NoteBook::RecreatePadTabs()
{
    SetSelection(0);
    if (GetPageCount() > 1)
    {
        while(GetPageCount()>1)
            RemovePage(1);
        SendSizeEvent();
    }

    for (int port = 0; port < 2; port++)
    {
        for (int slot = 0; slot < 4; slot++)
        {
            wxString title;

            if (config.padConfigs[port][slot].type == DisabledPad) continue;
            if (!GetPadName(title, port, slot)) continue;

            auto* m_pad_panel = new PadTab(this, port, slot);
            AddPage(m_pad_panel, title, false);
        }
    }
}

NoteBook::~NoteBook()
{

}

Dialog::Dialog()
	: wxDialog(nullptr, wxID_ANY, "Controller Config", wxDefaultPosition, wxSize(750, -1), wxCAPTION | wxCLOSE_BOX)
{
	auto* padding = new wxBoxSizer(wxVERTICAL);
	m_top_box = new wxBoxSizer(wxVERTICAL);

    book = new NoteBook(this);

    m_top_box->Add(book, wxSizerFlags().Centre().Expand());
    padding->Add(m_top_box, wxSizerFlags().Centre().Expand().Border(wxALL, 5));

	m_top_box->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Right());

	SetSizer(padding);
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
