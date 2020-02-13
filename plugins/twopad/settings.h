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

#include <string>
#include <cstring>

extern std::string ini_path;
extern std::string log_path;

void save_config();
void load_config();

// Code from https://stackoverflow.com/questions/17244181/reading-formatted-input-using-c-streams
template<class e, class t, int N>
std::basic_istream<e,t>& operator>>(std::basic_istream<e,t>& in, const e(&sliteral)[N]) {
        e buffer[N-1] = {}; //get buffer
        in >> buffer[0]; //skips whitespace
        if (N>2)
                in.read(buffer+1, N-2); //read the rest
        if (strncmp(buffer, sliteral, N-1)) //if it failed
                in.setstate(std::ios::failbit); //set the state
        return in;
}
template<class e, class t>
std::basic_istream<e,t>& operator>>(std::basic_istream<e,t>& in, const e& cliteral) {
        e buffer(0);  //get buffer
        in >> buffer; //read data
        if (buffer != cliteral) //if it failed
                in.setstate(std::ios::failbit); //set the state
        return in;
}
//redirect mutable char arrays to their normal function
template<class e, class t, int N>
std::basic_istream<e,t>& operator>>(std::basic_istream<e,t>& in, e(&carray)[N]) {
        return std::operator>>(in, carray);
}
