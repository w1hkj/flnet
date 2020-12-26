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

status progStatus = {
	50,								// int			mainX;
	50,								// int			mainY;
	SMALL,							// int			ui_size;

	FL_BLACK,						// int			fgColors[5];
	FL_BLACK,
	FL_WHITE,
	FL_YELLOW,
	FL_WHITE,
	FL_BLACK,						// int			bgColors[5];
	FL_WHITE,
	FL_BLUE,
	FL_DARK_GREEN,
	FL_DARK_RED,

	0,								// int			disp_new_login;
	0,								// int			open_editor;
	0,								// int			callin_is_up;
	1,								// int			QRZXML;

	'n',							// int			chAuto;
	true,							// int			left_justify

	"A",							// std::string	chP1;
	"B",							// std::string	chP2;
	"C", 							// std::string	chP3;

	"",								// std::string	strP1
	"",								// std::string	strP2
	"",								// std::string	strP3,
	"",								// std::string	myLocator;
	"",								// std::string	user_name;
	"",								// std::string	user_password;
	"https://www.callook.info/",	// std::string	callookurl;
	"https://www.hamqth.com/",		// std::string	hamqthurl;
	"https://www.hamcall.net/",		// std::string	hamcallurl;
	"https://www.qrz.com/",			// std::string	qrzurl;

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

	flnetpref.set("bgColors[0]", bgColors[0]);
	flnetpref.set("bgColors[1]", bgColors[1]);
	flnetpref.set("bgColors[2]", bgColors[2]);
	flnetpref.set("bgColors[3]", bgColors[3]);
	flnetpref.set("bgColors[4]", bgColors[4]);

	flnetpref.set("disp_new_login", disp_new_login);
	flnetpref.set("open_editor", open_editor);
	flnetpref.set("callin_is_up", callin_is_up);
	flnetpref.set("QRZXML", QRZXML);

	flnetpref.set("chAuto", chAuto);
	flnetpref.set("left_justify", left_justify);

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

	flnetpref.get("bgColors[0]", bgColors[0], bgColors[0]);
	flnetpref.get("bgColors[1]", bgColors[1], bgColors[1]);
	flnetpref.get("bgColors[2]", bgColors[2], bgColors[2]);
	flnetpref.get("bgColors[3]", bgColors[3], bgColors[3]);
	flnetpref.get("bgColors[4]", bgColors[4], bgColors[4]);

	flnetpref.get("disp_new_login", disp_new_login, disp_new_login);
	flnetpref.get("open_editor", open_editor, open_editor);
	flnetpref.get("callin_is_up", callin_is_up, callin_is_up);
	flnetpref.get("QRZXML", QRZXML, QRZXML);

	flnetpref.get("chAuto", chAuto, chAuto);
	flnetpref.get("left_justify", left_justify, left_justify);

	char sztext[100];
	flnetpref.get("chP1", sztext, chP1.c_str(), 100); chP1 = sztext;
	flnetpref.get("chP2", sztext, chP2.c_str(), 100); chP2 = sztext;
	flnetpref.get("chP3", sztext, chP3.c_str(), 100); chP3 = sztext;

	flnetpref.get("strP1", sztext, strP1.c_str(), 100); strP1 = sztext;
	flnetpref.get("strP2", sztext, strP2.c_str(), 100); strP2 = sztext;
	flnetpref.get("strP3", sztext, strP3.c_str(), 100); strP3 = sztext;

	flnetpref.get("myLocator", sztext, myLocator.c_str(), 100); myLocator = sztext;
	flnetpref.get("user_name", sztext, user_name.c_str(), 100); user_name = sztext;
	flnetpref.get("user_password", sztext, user_password.c_str(), 100); user_password = sztext;
	flnetpref.get("callookurl", sztext, callookurl.c_str(), 100); callookurl = sztext;
	flnetpref.get("hamqthurl", sztext, hamqthurl.c_str(), 100); hamqthurl = sztext;
	flnetpref.get("hamcallurl", sztext, hamcallurl.c_str(), 100); hamcallurl = sztext;
	flnetpref.get("qrzurl", sztext, qrzurl.c_str(), 100); qrzurl = sztext;
}
