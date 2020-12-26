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

using namespace std;

struct status {
	int		mainX;
	int		mainY;
	int		ui_size;

	int		fgColors[5];
	int		bgColors[5];

	int		disp_new_login;
	int		open_editor;
	int		callin_is_up;
	int		QRZXML;

	int				chAuto;
	int				left_justify;

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

	void saveLastState();
	void loadLastState();
};

extern status progStatus;

#endif
