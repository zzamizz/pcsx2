/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021 PCSX2 Dev Team
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

#include "Sio_PAD.h"
#include "Sio.h"

#ifdef _MSC_VER
u32 bufSize = 0;
unsigned char outBuf[50];
unsigned char inBuf[50];

void DEBUG_TEXT_OUT(const char* text)
{
	if (!config.debug)
		return;

	std::ofstream file("logs/padLog.txt", std::ios::app);
	if (!file.good())
		return;
	file << text;
}

void DEBUG_NEW_SET()
{
	if (config.debug && bufSize > 1)
	{
		std::ofstream file("logs/padLog.txt", std::ios::app);
		if (file.good())
		{
			std::stringstream stream;
			stream.setf(std::ios::hex, std::ios::basefield);
			stream.setf(std::ios::uppercase);
			stream.fill('0');

			unsigned char* buffer[2] = {inBuf, outBuf};
			for (const auto& buf : buffer)
			{
				// Port/FF
				stream << std::setw(2) << int(buf[0]);
				// Active slots/Enabled (only relevant for multitap)
				stream << " (" << std::setw(2) << int(buf[1]) << ')';

				// Command/Response
				for (u32 n = 2; n < bufSize; ++n)
					stream << ' ' << std::setw(2) << int(buf[n]);
				stream << '\n';
			}
			stream << '\n';
			file << stream.rdbuf();
		}
	}
	bufSize = 0;
}

inline void DEBUG_IN(unsigned char c)
{
	if (bufSize < sizeof(inBuf))
		inBuf[bufSize] = c;
}
inline void DEBUG_OUT(unsigned char c)
{
	if (bufSize < sizeof(outBuf))
		outBuf[bufSize++] = c;
}
#endif

//////////////////////////////////////////////////////////////////////
// QueryInfo implementation
//////////////////////////////////////////////////////////////////////

void QueryInfo::reset()
{
	port = 0;
	slot = 0;
	lastByte = 1;
	currentCommand = 0;
	numBytes = 0;
	queryDone = 1;
	memset(response, 0xF3, sizeof(response));
}

u8 QueryInfo::start_poll(int _port)
{
	if (port > 1)
	{
		reset();
		return 0;
	}

	queryDone = 0;
	port = _port;
	slot = sio.slot[port];
	numBytes = 2;
	lastByte = 0;

	return 0xFF;
}

#ifdef _MSC_VER
u8 PADstartPoll(int port)
{
	DEBUG_NEW_SET();
	port--;
	if ((unsigned int)port <= 1 && pads[port][sio.slot[port]].enabled)
	{
		query.queryDone = 0;
		query.port = port;
		query.slot = sio.slot[port];
		query.numBytes = 2;
		query.lastByte = 0;
		DEBUG_IN(port);
		DEBUG_OUT(0xFF);
		DEBUG_IN(sio.slot[port]);
		DEBUG_OUT(pads[port][sio.slot[port]].enabled);
		return 0xFF;
	}
	else
	{
		query.queryDone = 1;
		query.numBytes = 0;
		query.lastByte = 1;
		DEBUG_IN(0);
		DEBUG_OUT(0);
		DEBUG_IN(port);
		DEBUG_OUT(0);
		return 0;
	}
}
#else
u8 PADstartPoll(int pad)
{
	return query.start_poll(pad - 1);
}
#endif
