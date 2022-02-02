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
extern void adjust_priority();
extern void updateCallins (bool fldigi_flag = false);
extern void add_to_callins(int rnbr);
extern loglist callinlist;

extern std::string ascii_copy;

#define NPICKITEMS 32

enum MYGUI_STATUS {LOGLIST, SUFFIX, PREFIX, AREA, PICKLIST};

//class my_UI : public Fl_Group
class my_UI : public Fl_Input
{
private:
	MYGUI_STATUS my_status;
	//    int WhoIsUP;
	void clearSAP ();
	void fillPickList ();
	int  PickedToCallins (int);
	void PickedColors ();
	void clearPickList ();
public:
	my_UI (int x, int y, int w, int h, const char *l = 0);
	int handle (int);
	void dispCallIns (bool flag);
	void UpdateWhoIsUp (long L);
	int  PickedToCallinsDB(size_t);
};


#endif

