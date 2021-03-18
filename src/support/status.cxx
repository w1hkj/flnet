// ----------------------------------------------------------------------------
// Copyright (C) 2014
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

#include <iostream>
#include <fstream>
#include <string>

#include <FL/Fl_Preferences.H>

#include "config.h"

#include "status.h"
#include "netshared.h"
#include "masterdb.h"
#include "csvdb.h"

status progStatus = {
	50,			// int	mainX;
	50,			// int	mainY;
	SMALL,		// int	ui_size;

	FL_BLACK,	// int	fgColors[5];
	FL_BLACK,
	FL_WHITE,
	FL_YELLOW,
	FL_WHITE,
	FL_YELLOW,
	FL_WHITE,	// int	bgColors[5];
	FL_WHITE,
	FL_BLUE,
	FL_DARK_GREEN,
	FL_DARK_RED,
	FL_DARK_GREEN,

	0,			// int	disp_new_login;
	0,			// int	open_editor;
	0,			// int	callin_is_up;
	1,			// int	QRZXML;

	false,		// int	chAuto;
	1,			// int	call_justify, default = LEFT_JUSTIFY
	1,			// int	name_left_justify, default = LEFT_JUSTIFY
	0,			// int	distance conversion
				//		0 - KM, 1 - NM, 2 - SM

	2,			// int	preferred_sort_order
				//		0 - PAS
				//		1 - APS
				//		2 - SAP
	0,			// int	mdb_isopen;
	FL_GREEN,	// int	mdb_color;

	1,			// int	mdb_prefix;
	1,			// int	mdb_area;
	1,			// int	mdb_suffix;
	1,			// int	mdb_callsign;
	1,			// int	mdb_name;
	1,			// int	mdb_fname;
	1,			// int	mdb_lname;
	1,			// int	mdb_addr;
	1,			// int	mdb_city;
	1,			// int	mdb_state;
	1,			// int	mdb_zip;
	1,			// int	mdb_phone;
	1,			// int	mdb_birthdate;
	1,			// int	mdb_spouse;
	1,			// int	mdb_sp_birth;
	1,			// int	mdb_comment1;
	1,			// int	mdb_comment2;
	1,			// int	mdb_email;
	1,			// int	mdb_locator;
	1,			// int	mdb_country;
	1,			// int	mdb_logdate;
	1,			// int	mdb_prevdate;
	1,			// int	mdb_nbrlogins;
	1,			// int	mdb_status;
	1,			// int	mdb_joined;
	1,			// int	mdb_netnbr

	"A",		// std::string	chP1;
	"B",		// std::string	chP2;
	"C", 		// std::string	chP3;

	"",			// std::string	strP1
	"",			// std::string	strP2
	"",			// std::string	strP3,
	"",			// std::string	myLocator;
	"",			// std::string	user_name;
	"",			// std::string	user_password;

	"https://www.callook.info/",	// std::string	callookurl;
	"https://www.hamqth.com/",		// std::string	hamqthurl;
	"https://www.hamcall.net/",		// std::string	hamcallurl;
	"https://www.qrz.com/",			// std::string	qrzurl;
	""								// std::string	masterdb;

};

void status::saveLastState()
{
	Fl_Preferences flnetpref(home_dir.c_str(), "w1hkj.com",  PACKAGE_NAME);

	int mX = main_window->x();
	int mY = main_window->y();

	if (mX >= 0 && mX >= 0) {
		mainX = mX;
		mainY = mY;
	}
	flnetpref.set("version", PACKAGE_VERSION);
	flnetpref.set("mainx", mX);
	flnetpref.set("mainy", mY);
	flnetpref.set("ui_size", ui_size);

	flnetpref.set("fgColors[0]", fgColors[0]);
	flnetpref.set("fgColors[1]", fgColors[1]);
	flnetpref.set("fgColors[2]", fgColors[2]);
	flnetpref.set("fgColors[3]", fgColors[3]);
	flnetpref.set("fgColors[4]", fgColors[4]);
	flnetpref.set("fgColors[5]", fgColors[5]);

	flnetpref.set("bgColors[0]", bgColors[0]);
	flnetpref.set("bgColors[1]", bgColors[1]);
	flnetpref.set("bgColors[2]", bgColors[2]);
	flnetpref.set("bgColors[3]", bgColors[3]);
	flnetpref.set("bgColors[4]", bgColors[4]);
	flnetpref.set("bgColors[5]", bgColors[5]);

	flnetpref.set("disp_new_login", disp_new_login);
	flnetpref.set("open_editor", open_editor);
	flnetpref.set("callin_is_up", callin_is_up);
	flnetpref.set("QRZXML", QRZXML);
	flnetpref.set("preferred_sort_order", preferred_sort_order);

	flnetpref.set("mdb_prefix", mdb_prefix);
	flnetpref.set("mdb_area", mdb_area);
	flnetpref.set("mdb_suffix", mdb_suffix);
	flnetpref.set("mdb_callsign", mdb_callsign);
	flnetpref.set("mdb_name", mdb_name);
	flnetpref.set("mdb_fname", mdb_fname);
	flnetpref.set("mdb_lname", mdb_lname);
	flnetpref.set("mdb_addr", mdb_addr);
	flnetpref.set("mdb_city", mdb_city);
	flnetpref.set("mdb_state", mdb_state);
	flnetpref.set("mdb_zip", mdb_zip);
	flnetpref.set("mdb_phone", mdb_phone);
	flnetpref.set("mdb_birthdate", mdb_birthdate);
	flnetpref.set("mdb_spouse", mdb_spouse);
	flnetpref.set("mdb_sp_birth", mdb_sp_birth);
	flnetpref.set("mdb_comment1", mdb_comment1);
	flnetpref.set("mdb_comment2", mdb_comment2);
	flnetpref.set("mdb_email", mdb_email);
	flnetpref.set("mdb_locator", mdb_locator);
	flnetpref.set("mdb_country", mdb_country);
	flnetpref.set("mdb_logdate", mdb_logdate);
	flnetpref.set("mdb_prevdate", mdb_prevdate);
	flnetpref.set("mdb_nbrlogins", mdb_nbrlogins);
	flnetpref.set("mdb_status", mdb_status);
	flnetpref.set("mdb_joined", mdb_joined);
	flnetpref.set("mdb_netnbr", mdb_netnbr);

	flnetpref.set("mdb_isopen", mdb_isopen);
	flnetpref.set("mdb_color", mdb_color);

	flnetpref.set("chAuto", chAuto);

	flnetpref.set("call_justify", call_justify);
	flnetpref.set("name_justify", name_justify);

	flnetpref.set("chP1", chP1.c_str());
	flnetpref.set("chP2", chP2.c_str());
	flnetpref.set("chP3", chP3.c_str());

	flnetpref.set("strP1", strP1.c_str());
	flnetpref.set("strP2", strP2.c_str());
	flnetpref.set("strP3", strP3.c_str());

	flnetpref.set("myLocator", myLocator.c_str());
	flnetpref.set("user_name", user_name.c_str());
	flnetpref.set("user_password", user_password.c_str());
	flnetpref.set("callookurl", callookurl.c_str());
	flnetpref.set("hamqthurl", hamqthurl.c_str());
	flnetpref.set("hamcallurl", hamcallurl.c_str());
	flnetpref.set("qrzurl", qrzurl.c_str());
	flnetpref.set("masterdb", masterdb.c_str());

	flnetpref.set("arc_conversion", arc_conversion);
}

void status::loadLastState()
{
	Fl_Preferences flnetpref(home_dir.c_str(), "w1hkj.com", PACKAGE_NAME);

	if (flnetpref.entryExists("version")) {
		flnetpref.get("mainx", mainX, mainX);
		flnetpref.get("mainy", mainY, mainY);
		flnetpref.get("ui_size", ui_size, ui_size);
	}


	flnetpref.get("fgColors[0]", fgColors[0], fgColors[0]);
	flnetpref.get("fgColors[1]", fgColors[1], fgColors[1]);
	flnetpref.get("fgColors[2]", fgColors[2], fgColors[2]);
	flnetpref.get("fgColors[3]", fgColors[3], fgColors[3]);
	flnetpref.get("fgColors[4]", fgColors[4], fgColors[4]);
	flnetpref.get("fgColors[5]", fgColors[5], fgColors[5]);

	flnetpref.get("bgColors[0]", bgColors[0], bgColors[0]);
	flnetpref.get("bgColors[1]", bgColors[1], bgColors[1]);
	flnetpref.get("bgColors[2]", bgColors[2], bgColors[2]);
	flnetpref.get("bgColors[3]", bgColors[3], bgColors[3]);
	flnetpref.get("bgColors[4]", bgColors[4], bgColors[4]);
	flnetpref.get("bgColors[5]", bgColors[5], bgColors[5]);

	flnetpref.get("disp_new_login", disp_new_login, disp_new_login);
	flnetpref.get("open_editor", open_editor, open_editor);
	flnetpref.get("callin_is_up", callin_is_up, callin_is_up);
	flnetpref.get("QRZXML", QRZXML, QRZXML);
	flnetpref.get("preferred_sort_order", preferred_sort_order, preferred_sort_order);

	flnetpref.get("mdb_prefix", mdb_prefix, mdb_prefix);
	flnetpref.get("mdb_area", mdb_area, mdb_area);
	flnetpref.get("mdb_suffix", mdb_suffix, mdb_suffix);
	flnetpref.get("mdb_callsign", mdb_callsign, mdb_callsign);
	flnetpref.get("mdb_name", mdb_name, mdb_name);
	flnetpref.get("mdb_fname", mdb_fname, mdb_fname);
	flnetpref.get("mdb_lname", mdb_lname, mdb_lname);
	flnetpref.get("mdb_addr", mdb_addr, mdb_addr);
	flnetpref.get("mdb_city", mdb_city, mdb_city);
	flnetpref.get("mdb_state", mdb_state, mdb_state);
	flnetpref.get("mdb_zip", mdb_zip, mdb_zip);
	flnetpref.get("mdb_phone", mdb_phone, mdb_phone);
	flnetpref.get("mdb_birthdate", mdb_birthdate, mdb_birthdate);
	flnetpref.get("mdb_spouse", mdb_spouse, mdb_spouse);
	flnetpref.get("mdb_sp_birth", mdb_sp_birth, mdb_sp_birth);
	flnetpref.get("mdb_comment1", mdb_comment1, mdb_comment1);
	flnetpref.get("mdb_comment2", mdb_comment2, mdb_comment2);
	flnetpref.get("mdb_email", mdb_email, mdb_email);
	flnetpref.get("mdb_locator", mdb_locator, mdb_locator);
	flnetpref.get("mdb_country", mdb_country, mdb_country);
	flnetpref.get("mdb_logdate", mdb_logdate, mdb_logdate);
	flnetpref.get("mdb_prevdate", mdb_prevdate, mdb_prevdate);
	flnetpref.get("mdb_nbrlogins", mdb_nbrlogins, mdb_nbrlogins);
	flnetpref.get("mdb_status", mdb_status, mdb_status);
	flnetpref.get("mdb_joined", mdb_joined, mdb_joined);
	flnetpref.get("mdb_netnbr", mdb_netnbr, mdb_netnbr);

	flnetpref.get("mdb_isopen", mdb_isopen, mdb_isopen);
	flnetpref.get("mdb_color", mdb_color, mdb_color);

	flnetpref.get("chAuto", chAuto, chAuto);

	flnetpref.get("call_justify", call_justify, call_justify);
	flnetpref.get("name_justify", name_justify, name_justify);

	char sztext[501];
	flnetpref.get("chP1", sztext, chP1.c_str(), 500); chP1 = sztext;
	flnetpref.get("chP2", sztext, chP2.c_str(), 500); chP2 = sztext;
	flnetpref.get("chP3", sztext, chP3.c_str(), 500); chP3 = sztext;

	flnetpref.get("strP1", sztext, strP1.c_str(), 500); strP1 = sztext;
	flnetpref.get("strP2", sztext, strP2.c_str(), 500); strP2 = sztext;
	flnetpref.get("strP3", sztext, strP3.c_str(), 500); strP3 = sztext;

	flnetpref.get("myLocator", sztext, myLocator.c_str(), 500); myLocator = sztext;
	flnetpref.get("user_name", sztext, user_name.c_str(), 500); user_name = sztext;
	flnetpref.get("user_password", sztext, user_password.c_str(), 500); user_password = sztext;
	flnetpref.get("callookurl", sztext, callookurl.c_str(), 500); callookurl = sztext;
	flnetpref.get("hamqthurl", sztext, hamqthurl.c_str(), 500); hamqthurl = sztext;
	flnetpref.get("hamcallurl", sztext, hamcallurl.c_str(), 500); hamcallurl = sztext;
	flnetpref.get("qrzurl", sztext, qrzurl.c_str(), 500); qrzurl = sztext;
	flnetpref.get("masterdb", sztext, masterdb.c_str(), 500); masterdb = sztext;

	flnetpref.get("arc_conversion", arc_conversion, arc_conversion);
}
