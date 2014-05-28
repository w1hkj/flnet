//======================================================================
// my_UI.h
//
// Authors:
//
// Copyright (C) 2013, Dave Freese, W1HKJ
//
// This file is part of FLNET.
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//======================================================================

#ifndef _MY_UI
#define _MY_UI

#include <FL/Fl_Group.H>
#include "loglist.h"

extern int WhoIsUp;
extern void updateCallins (bool);
extern loglist callinlist;

enum MYGUI_STATUS {LOGLIST, SUFFIX, PREFIX, AREA, PICKLIST};

class my_UI : public Fl_Group
{
private:
	MYGUI_STATUS my_status;
	//    int WhoIsUP;
	void clearSAP ();
	void fillPickList ();
	void PickedToCallins (int);
	void PickedColors ();
	void clearPickList ();
public:
	my_UI (int x, int y, int w, int h, const char *l = 0);
	int handle (int);
	void dispCallIns (bool flag);
	void UpdateWhoIsUp (long L);
	void PickedToCallinsDB(size_t);
};

#endif

