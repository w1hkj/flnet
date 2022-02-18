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

#ifndef _status_H
#define _status_H

#include <string>

enum { RIGHT_JUSTIFY, LEFT_JUSTIFY, AREA_JUSTIFY };

struct status {
	int		mainX;
	int		mainY;
	int		ui_size;

	int		fgColors[5];
	int		bgColors[5];

	std::string f1_text;
	std::string f2_text;
	std::string f3_text;
	std::string f4_text;

	int		disp_new_login;
	int		open_editor;
	int		callin_is_up;
	int		QRZXML;

	int		chAuto;
	int		call_justify;
	int		name_justify;
	int		arc_conversion;

	int		preferred_sort_order;
	int		mdb_isopen;
	int		mdb_color;

	int		mdb_prefix;
	int		mdb_area;
	int		mdb_suffix;
	int		mdb_callsign;
	int		mdb_name;
	int		mdb_fname;
	int		mdb_lname;
	int		mdb_addr;
	int		mdb_city;
	int		mdb_state;
	int		mdb_zip;
	int		mdb_phone;
	int		mdb_birthdate;
	int		mdb_spouse;
	int		mdb_sp_birth;
	int		mdb_comment1;
	int		mdb_comment2;
	int		mdb_email;
	int		mdb_locator;
	int		mdb_country;
	int		mdb_logdate;
	int		mdb_prevdate;
	int		mdb_nbrlogins;
	int		mdb_status;
	int		mdb_joined;
	int		mdb_netnbr;
	int		mdb_county;
	int		mdb_traffic;

	int		chk0; std::string header0;
	int		chk1; std::string header1;
	int		chk2; std::string header2;
	int		chk3; std::string header3;
	int		chk4; std::string header4;
	int		chk5; std::string header5;
	int		chk6; std::string header6;
	int		chk7; std::string header7;
	int		chk8; std::string header8;
	int		chk9; std::string header9;
	int		chk10; std::string header10;
	int		chk11; std::string header11;
	int		chk12; std::string header12;
	int		chk13; std::string header13;
	int		chk14; std::string header14;
	int		chk15; std::string header15;
	int		chk16; std::string header16;
	int		chk17; std::string header17;
	int		chk18; std::string header18;
	int		chk19; std::string header19;
	int		chk20; std::string header20;
	int		chk21; std::string header21;
	int		chk22; std::string header22;
	int		chk23; std::string header23;
	int		chk24; std::string header24;
	int		chk25; std::string header25;
	int		chk26; std::string header26;
	int		chk27; std::string header27;
	int		chk28; std::string header28;

	int		column_char;
	int		col_spaces;

	std::string		chP1;
	std::string		chP2;
	std::string		chP3;

	std::string		strP1;
	std::string		strP2;
	std::string		strP3;

	std::string		myLocator;
	std::string		user_name;
	std::string		user_password;
	std::string		callookurl;
	std::string		hamqthurl;
	std::string		hamcallurl;
	std::string		qrzurl;
	std::string		masterdb;

	void saveLastState();
	void loadLastState();
};

extern status progStatus;

#endif
