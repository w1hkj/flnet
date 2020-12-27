//======================================================================
// netshared.h
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

#ifndef _NETSHARED
#define _NETSHARED

#include <string>

#include "net_ui.h"
#include "netutils.h"
#include "netedits.h"

#include "csvdb.h"

extern std::string home_dir;
extern std::string selected_file;

extern csvdb netdb;

struct brwsStruct {
	int  recN;
	char prefix[4];
	char area[2];
	char suffix[4];
	char netnbr[5];
};

enum sizes {SMALL, BIG};
enum State {NEW, UPDATE, ADD, MODIFY};
enum schema {ORIG, EXTENDED};

extern Fl_Window *main_window;
//extern Fl_Window *editor;

extern brwsStruct *brwsData;
extern State editstate;

extern size_t currec;
extern size_t brwsnum;

extern char sSimpleName[];

extern Fl_Input *inpPrefix;

extern void saveCurRecord ();
extern void clearEditForm ();
extern void appendNewRecord ();

extern std::string uppercase (std::string s);
extern std::string trim (std::string s);

extern void toggleState ();

extern void getBrwsData ();
extern int NetNbrCompare (const void *, const void *);
extern int SAPCompare (const void *, const void *);
extern int APSCompare (const void *, const void *);
extern void AddNewRecord (char *prefix, char *area, char *suffix);
extern void setAddState ();

extern void SortBySAP ();
extern void SortByNetNbr ();
extern void SortByAPS ();

extern void closeDB ();
extern void openDB (char *);
extern void dispRec ();
extern void clearEditForm ();
extern void setFields ();
extern void saveCurRecord ();
extern void appendNewRecord ();
extern void appendNewRecord (csvRecord &rec);
extern int  add_fldigi_record(void);

extern void cb_F12 (int);
extern void cb_ShiftF12(void);
extern void cbEditor ();
extern void cbCloseEditor ();
extern void cbConfig ();
extern void cbCloseConfig ();

extern void ui_size(int, int, int);
extern void change_size();
extern bool split_call(std::string src, std::string &p, std::string &a, std::string &s);

#endif

