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
	50,				// int mainX;
	50,				// int mainY;
	SMALL			// int ui_size;
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
}

void status::loadLastState()
{
	Fl_Preferences flnetpref(home_dir.c_str(), "w1hkj.com", PACKAGE_NAME);

	if (flnetpref.entryExists("version")) {
		flnetpref.get("mainx", mainX, mainX);
		flnetpref.get("mainy", mainY, mainY);
		flnetpref.get("ui_size", ui_size, ui_size);
	}
}
