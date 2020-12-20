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

#pragma once

#include "Global.h"
#include "mt_queue.h"

#ifdef SDL_BUILD
extern void UpdateSDLDevices();
#endif

extern FILE* padLog;
extern void initLogging();

extern keyEvent event;
extern MtQueue<keyEvent> g_ev_fifo;

extern s32 _PADopen(void* pDsp);
extern void _PADclose();
extern void PADsetMode(int pad, int mode);

extern void __LogToConsole(const char* fmt, ...);
extern void PADLoadConfig();
extern void PADSaveConfig();

extern void SysMessage(char* fmt, ...);

extern s32 PADinit();
extern void PADshutdown();
extern s32 PADopen(void* pDsp);
extern void PADsetLogDir(const char* dir);
extern void PADclose();
extern u32 PADquery();
extern s32 PADsetSlot(u8 port, u8 slot);
extern s32 PADfreeze(int mode, freezeData* data);
extern u8 PADstartPoll(int pad);
extern u8 PADpoll(u8 value);
extern keyEvent* PADkeyEvent();
void PADupdate(int pad);
extern void PADconfigure();
extern void PADDoFreezeOut(void* dest);
extern void PADDoFreezeIn(pxInputStream& infp);

#if defined(__unix__)
extern void PADWriteEvent(keyEvent& evt);
#endif
