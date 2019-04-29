//======================================================================
// netedits.h
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

#ifndef netedits_h
#define netedits_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "date.h"
#include "calendar.h"

Fl_Window *newEditWindow(bool);
Fl_Window *getEditWindow(void);

#include <FL/Fl_Menu_Bar.H>
extern Fl_Menu_Bar *mbarMain;
extern void cb_mnuSearchCallsign(Fl_Menu_*, void*);
extern void cb_mnuSearchNetNbr(Fl_Menu_*, void*);
extern void cb_mnuBrowseCallsign(Fl_Menu_*, void*);
extern void cb_mnuBrowseNetNbr(Fl_Menu_*, void*);
extern void cb_mnuFldigiEditor(Fl_Menu_*, void*);
extern void cb_LookupCall(Fl_Menu_*, void*);

#include <FL/Fl_Input.H>
extern Fl_Input *inpPrefix;
extern Fl_Input *inpArea;
extern Fl_Input *inpSuffix;
extern Fl_Input *inpNickname;
extern Fl_Input *inpNetNbr;
extern Fl_Input *inpFname;
extern Fl_Input *inpLname;
extern Fl_Input *inpAddress;
extern Fl_Input *inpCity;
extern Fl_Input *inpState;
extern Fl_Input *inpZip;
extern Fl_Input *inpPhone;
extern Fl_DateInput *inpBirthday;
extern Fl_Input *inpSpouse;
extern Fl_Input *inpSpBirthday;

#include <FL/Fl_Output.H>
extern Fl_DateInput *txtLogDate;

#include <FL/Fl_Button.H>
extern void cbGoFirstRec(Fl_Button*, void*);
extern Fl_Button *btnFirst;
extern void cbGoPrevRec(Fl_Button*, void*);
extern Fl_Button *btnPrev;
extern void cbGoNextRec(Fl_Button*, void*);
extern Fl_Button *btnNext;
extern void cbGoLastRec(Fl_Button*, void*);
extern Fl_Button *btnLast;
extern void cb_btnNewSave(Fl_Button*, void*);
extern Fl_Button *btnNewSave;
extern void cb_btnDelete(Fl_Button*, void*);
extern Fl_Button *btnDelete;
extern void cb_btnUpdateCancel(Fl_Button*, void*);
extern Fl_Button *btn2Queue;
extern void cb_btn2Queue(Fl_Button *, void *);
extern Fl_Button *btnUpdateCancel;
extern void cb_CloseEditor(Fl_Button*, void*);
extern Fl_Button *btnClose;
extern Fl_Output *lblFileName;
extern Fl_Output *lblNumRecs;
extern Fl_Input *inpCallsign;
extern Fl_DateInput *inpPrevDate;
extern Fl_Input *inpNbrLogins;
extern Fl_Input *inpStatus;
extern Fl_DateInput *inpJoined;
extern Fl_Input *inpEmail;
extern Fl_Input *inpComment1;
extern Fl_Input *inpComment2;

extern Fl_Menu_Item menu_mbarMain[];

#define mnuSort (menu_mbarMain+0)
#define mnuSortByAPS (menu_mbarMain+1)
#define mnuSortBySAP (menu_mbarMain+2)
#define mnuSortByNetNbr (menu_mbarMain+3)
#define mnuSearch (menu_mbarMain+5)
#define mnuSearchCallsign (menu_mbarMain+6)
#define mnuSearchNetNbr (menu_mbarMain+7)
#define mnuBrowse (menu_mbarMain+9)
#define mnuBrowseCallsign (menu_mbarMain+10)
#define mnuBrowseNetNbr (menu_mbarMain+11)

extern void close_editor(void);

#endif
