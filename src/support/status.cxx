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

#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>

#include "config.h"

#include "status.h"
#include "netshared.h"
#include "masterdb.h"
#include "csvdb.h"
#include "my_UI.h"

#include "sorting.h"

status progStatus = {
	50,			// int	mainX;
	50,			// int	mainY;
	SMALL,		// int	ui_size;
	4,			// int	ui_fnt; default mono font

	FL_BLACK,	// int	fgColors[5];
	FL_WHITE,
	FL_YELLOW,
	FL_WHITE,
	FL_YELLOW,
	FL_WHITE,	// int	bgColors[5];
	FL_BLUE,
	FL_DARK_GREEN,
	FL_DARK_RED,
	FL_DARK_GREEN,

	"First",	// std::string f1_text;
	"Second",	// std::string f1_text;
	"Third",	// std::string f1_text;
	"Fourth",	// std::string f1_text;

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
	1,			// int	mdb_county;
	1,			// int	mdb_traffic;

	1, "CALLSIGN",	// int chk0; std::string header0;
	1, "NAME",		// int chk1; std::string header1;
	1, "NETNBR",	// int chk2; std::string header2;
	0, "LOGDATE",	// int chk3; std::string header3;
	1, "LOGTIME",	// int chk4; std::string header4;
	0, "PREVDATE",	// int chk5; std::string header5;
	0, "NBRLOGINS",	// int chk6; std::string header6;
	1, "STATUS",	// int chk7; std::string header7;
	0, "PREFIX",	// int chk8; std::string header8;
	0, "AREA",		// int chk9; std::string header9;
	0, "SUFFIX",	// int chk10; std::string header10;
	0, "FNAME",		// int chk11; std::string header11;
	0, "LNAME",		// int chk12; std::string header12;
	0, "ADDR",		// int chk13; std::string header13;
	0, "CITY",		// int chk14; std::string header14;
	0, "STATE",		// int chk15; std::string header15;
	0, "ZIP",		// int chk16; std::string header16;
	0, "PHONE",		// int chk17; std::string header17;
	0, "BIRTHDATE",	// int chk18; std::string header18;
	0, "JOINED",	// int chk19; std::string header19;
	0, "EMAIL",		// int chk20; std::string header20;
	0, "LOCATOR",	// int chk21; std::string header21;
	0, "COUNTRY",	// int chk22; std::string header22;
	0, "COUNTY",	// int chk23; std::string header23;
	0, "TRAFFIC",	// int chk24; std::string header24;
	0, "SPOUSE",	// int chk24; std::string header25;
	0, "SP_BIRTH",	// int chk25; std::string header26;
	0, "COMMENT1",	// int chk26; std::string header27;
	0, "COMMENT2",	// int chk27; std::string header28;

	'\t',		// char	column_char;
	2,			// int	col_spaces;

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

	save_column_select();

	flnetpref.set("version", PACKAGE_VERSION);
	flnetpref.set("mainx", mX);
	flnetpref.set("mainy", mY);
	flnetpref.set("ui_size", ui_size);
	flnetpref.set("ui_font", ui_font);

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

	flnetpref.set("chk0", chk0); flnetpref.set("header0", header0.c_str());
	flnetpref.set("chk1", chk1); flnetpref.set("header1", header1.c_str());
	flnetpref.set("chk2", chk2); flnetpref.set("header2", header2.c_str());
	flnetpref.set("chk3", chk3); flnetpref.set("header3", header3.c_str());
	flnetpref.set("chk4", chk4); flnetpref.set("header4", header4.c_str());
	flnetpref.set("chk5", chk5); flnetpref.set("header5", header5.c_str());
	flnetpref.set("chk6", chk6); flnetpref.set("header6", header6.c_str());
	flnetpref.set("chk7", chk7); flnetpref.set("header7", header7.c_str());
	flnetpref.set("chk8", chk8); flnetpref.set("header8", header8.c_str());
	flnetpref.set("chk9", chk9); flnetpref.set("header9", header9.c_str());
	flnetpref.set("chk10", chk10); flnetpref.set("header10", header10.c_str());
	flnetpref.set("chk11", chk11); flnetpref.set("header11", header11.c_str());
	flnetpref.set("chk12", chk12); flnetpref.set("header12", header12.c_str());
	flnetpref.set("chk13", chk13); flnetpref.set("header13", header13.c_str());
	flnetpref.set("chk14", chk14); flnetpref.set("header14", header14.c_str());
	flnetpref.set("chk15", chk15); flnetpref.set("header15", header15.c_str());
	flnetpref.set("chk16", chk16); flnetpref.set("header16", header16.c_str());
	flnetpref.set("chk17", chk17); flnetpref.set("header17", header17.c_str());
	flnetpref.set("chk18", chk18); flnetpref.set("header18", header18.c_str());
	flnetpref.set("chk19", chk19); flnetpref.set("header19", header19.c_str());
	flnetpref.set("chk20", chk20); flnetpref.set("header20", header20.c_str());
	flnetpref.set("chk21", chk21); flnetpref.set("header21", header21.c_str());
	flnetpref.set("chk22", chk22); flnetpref.set("header22", header22.c_str());
	flnetpref.set("chk23", chk23); flnetpref.set("header23", header23.c_str());
	flnetpref.set("chk24", chk24); flnetpref.set("header24", header24.c_str());
	flnetpref.set("chk25", chk25); flnetpref.set("header25", header25.c_str());
	flnetpref.set("chk26", chk26); flnetpref.set("header26", header26.c_str());
	flnetpref.set("chk27", chk27); flnetpref.set("header27", header27.c_str());
	flnetpref.set("chk28", chk28); flnetpref.set("header28", header28.c_str());

	flnetpref.set("column_char", column_char);
	flnetpref.set("col_spaces", col_spaces);

	flnetpref.set("chAuto", chAuto);

	flnetpref.set("call_justify", call_justify);
	flnetpref.set("name_justify", name_justify);

	flnetpref.set("chP1", chP1.c_str());
	flnetpref.set("chP2", chP2.c_str());
	flnetpref.set("chP3", chP3.c_str());

	flnetpref.set("strP1", strP1.c_str());
	flnetpref.set("strP2", strP2.c_str());
	flnetpref.set("strP3", strP3.c_str());

	flnetpref.set("f1_text", f1_text.c_str());
	flnetpref.set("f2_text", f2_text.c_str());
	flnetpref.set("f3_text", f3_text.c_str());
	flnetpref.set("f4_text", f4_text.c_str());

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
	char sztext[501];

	Fl_Preferences flnetpref(home_dir.c_str(), "w1hkj.com", PACKAGE_NAME);

	if (!flnetpref.entryExists("version")) 
		return;

	flnetpref.get("mainx", mainX, mainX);
	flnetpref.get("mainy", mainY, mainY);
	flnetpref.get("ui_size", ui_size, ui_size);
	flnetpref.get("ui_font", ui_font, ui_font);

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

	flnetpref.get("chk0", chk0, chk0);
	flnetpref.get("header0", sztext, header0.c_str(), 500); header0 = sztext;

	flnetpref.get("chk1", chk1, chk1);
	flnetpref.get("header1", sztext, header1.c_str(), 500); header1 = sztext;

	flnetpref.get("chk2", chk2, chk2);
	flnetpref.get("header2", sztext, header2.c_str(), 500); header2 = sztext;

	flnetpref.get("chk3", chk3, chk3);
	flnetpref.get("header3", sztext, header3.c_str(), 500); header3 = sztext;

	flnetpref.get("chk4", chk4, chk4);
	flnetpref.get("header4", sztext, header4.c_str(), 500); header4 = sztext;

	flnetpref.get("chk5", chk5, chk5);
	flnetpref.get("header5", sztext, header5.c_str(), 500); header5 = sztext;

	flnetpref.get("chk6", chk6, chk6);
	flnetpref.get("header6", sztext, header6.c_str(), 500); header6 = sztext;

	flnetpref.get("chk7", chk7, chk7);
	flnetpref.get("header7", sztext, header7.c_str(), 500); header7 = sztext;

	flnetpref.get("chk8", chk8, chk8);
	flnetpref.get("header8", sztext, header8.c_str(), 500); header8 = sztext;

	flnetpref.get("chk9", chk9, chk9);
	flnetpref.get("header9", sztext, header9.c_str(), 500); header9 = sztext;

	flnetpref.get("chk10", chk10, chk10);
	flnetpref.get("header10", sztext, header10.c_str(), 500); header10 = sztext;

	flnetpref.get("chk11", chk11, chk11);
	flnetpref.get("header11", sztext, header11.c_str(), 500); header11 = sztext;

	flnetpref.get("chk12", chk12, chk12);
	flnetpref.get("header12", sztext, header12.c_str(), 500); header12 = sztext;

	flnetpref.get("chk13", chk13, chk13);
	flnetpref.get("header13", sztext, header13.c_str(), 500); header13 = sztext;

	flnetpref.get("chk14", chk14, chk14);
	flnetpref.get("header14", sztext, header14.c_str(), 500); header14 = sztext;

	flnetpref.get("chk15", chk15, chk15);
	flnetpref.get("header15", sztext, header15.c_str(), 500); header15 = sztext;

	flnetpref.get("chk16", chk16, chk16);
	flnetpref.get("header16", sztext, header16.c_str(), 500); header16 = sztext;

	flnetpref.get("chk17", chk17, chk17);
	flnetpref.get("header17", sztext, header17.c_str(), 500); header17 = sztext;

	flnetpref.get("chk18", chk18, chk18);
	flnetpref.get("header18", sztext, header18.c_str(), 500); header18 = sztext;

	flnetpref.get("chk19", chk19, chk19);
	flnetpref.get("header19", sztext, header19.c_str(), 500); header19 = sztext;

	flnetpref.get("chk20", chk20, chk20);
	flnetpref.get("header20", sztext, header20.c_str(), 500); header20 = sztext;

	flnetpref.get("chk21", chk21, chk21);
	flnetpref.get("header21", sztext, header21.c_str(), 500); header21 = sztext;

	flnetpref.get("chk22", chk22, chk22);
	flnetpref.get("header22", sztext, header22.c_str(), 500); header22 = sztext;

	flnetpref.get("chk23", chk23, chk23);
	flnetpref.get("header23", sztext, header23.c_str(), 500); header23 = sztext;

	flnetpref.get("chk24", chk24, chk24);
	flnetpref.get("header24", sztext, header24.c_str(), 500); header24 = sztext;

	flnetpref.get("chk25", chk25, chk25);
	flnetpref.get("header25", sztext, header25.c_str(), 500); header25 = sztext;

	flnetpref.get("chk26", chk26, chk26);
	flnetpref.get("header26", sztext, header26.c_str(), 500); header26 = sztext;

	flnetpref.get("chk27", chk27, chk27);
	flnetpref.get("header27", sztext, header27.c_str(), 500); header27 = sztext;

	flnetpref.get("chk28", chk28, chk28);
	flnetpref.get("header28", sztext, header28.c_str(), 500); header28 = sztext;

	flnetpref.get("column_char", column_char, column_char);
	flnetpref.get("col_spaces", col_spaces, col_spaces);

	flnetpref.get("chAuto", chAuto, chAuto);
	callinlist.AutoPriority(chAuto);

	flnetpref.get("call_justify", call_justify, call_justify);
	flnetpref.get("name_justify", name_justify, name_justify);

	flnetpref.get("f1_text", sztext, f1_text.c_str(), 500); f1_text = sztext;
	flnetpref.get("f2_text", sztext, f2_text.c_str(), 500); f2_text = sztext;
	flnetpref.get("f3_text", sztext, f3_text.c_str(), 500); f3_text = sztext;
	flnetpref.get("f4_text", sztext, f4_text.c_str(), 500); f4_text = sztext;

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

	load_column_select();

}
