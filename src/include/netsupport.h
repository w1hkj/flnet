//======================================================================
// netsupport.h
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

#ifndef _NETSUPPORT
#define _NETSUPPORT

#include <string>

extern void openDB(std::string);
extern void dispRec();
extern void closeDB();

extern void cleanExit();
extern void cb_btnNewSave (Fl_Button *, void *);
extern void cb_btnDelete (Fl_Button *, void *);
extern void cb_btnUpdateCancel (Fl_Button *, void *);
extern void cb_mnuConfig (Fl_Menu_ *, void *);
extern void cb_btnCloseConfig (Fl_Return_Button *, void *);

extern void cb_mnuMigrate (Fl_Menu_ *, void*);

extern void visit_URL(void *);

extern void open_log_ins();
extern void copy_to_clipboard();

#endif

