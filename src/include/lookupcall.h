// ----------------------------------------------------------------------------
// Copyright (C) 2014-2109-2019
//              David Freese, W1HKJ
//
// This file is part of flnet
//
// flnet is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flnet is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef LOOKUPCALL_H
#define LOOKUPCALL_H

#include <cstring>

extern std::string lookup_latd;
extern std::string lookup_lond;
extern std::string lookup_addr1;
extern std::string lookup_addr2;
extern std::string lookup_qth;
extern std::string lookup_state;
extern std::string lookup_province;
extern std::string lookup_zip;
extern std::string lookup_country;

extern void clear_Lookup();
extern void compute();

extern void CALLSIGNquery(std::string inpCall);

enum qrz_xmlquery_t { 
QRZXML_EXIT = -1, 
QRZXMLNONE, 
QRZNET,
HAMCALLNET,
CALLOOK, 
HAMQTH };

//enum qrz_webquery_t { 
//QRZWEB_EXIT = -1, 
//QRZWEBNONE, 
//QRZHTML, HAMCALLHTML, HAMQTHHTML, CALLOOKHTML };

struct PROGDEFAULTS {
	std::string myLocator;
	std::string user_name;
	std::string user_password;
	std::string callookurl;
	std::string hamqthurl;
	std::string hamcallurl;
	std::string qrzurl;
	int QRZXML;
};

extern PROGDEFAULTS progdefaults;

#endif
