// =====================================================================
//
// netedits.cxx
//
// Authors:
//
// Copyright (C) 2012, Dave Freese, W1HKJ
// Copyright (C) 2014, Robert Stiles, KK5VD
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
// =====================================================================
// Editor dialogs for both 16 & 24 field database structures


#include "netedits.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netshared.h"
#include "netutils.h"
#include "lookupcall.h"
#include "date.h"
#include "calendar.h"

static void cb_mnuSortByAPS(Fl_Menu_*, void*) {
	SortByAPS ();
	cbGoFirstRec (NULL,NULL);
}

static void cb_mnuSortBySAP(Fl_Menu_*, void*) {
	SortBySAP ();
	cbGoFirstRec (NULL,NULL);
}

static void cb_mnuSortByNetNbr(Fl_Menu_*, void*) {
	SortByNetNbr ();
	cbGoFirstRec (NULL,NULL);
}

Fl_Menu_Item menu_mbarMain[] = {
	{"&Sort", 0,  0, 0, 64, 0, 0, 14, 56},
	{"&Area/Prefix/Suffix", 0,  (Fl_Callback*)cb_mnuSortByAPS, 0, 0, 0, 0, 14, 56},
	{"&Suffix/Area/Prefix", 0,  (Fl_Callback*)cb_mnuSortBySAP, 0, 0, 0, 0, 14, 56},
	{"&Net Number", 0,  (Fl_Callback*)cb_mnuSortByNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"Sea&rch", 0,  0, 0, 64, 0, 0, 14, 56},
	{"&Callsign", 0,  (Fl_Callback*)cb_mnuSearchCallsign, 0, 0, 0, 0, 14, 56},
	{"&NetNbr", 0,  (Fl_Callback*)cb_mnuSearchNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Browse", 0,  0, 0, 64, 0, 0, 14, 56},
	{"&Callsign", 0,  (Fl_Callback*)cb_mnuBrowseCallsign, 0, 0, 0, 0, 14, 56},
	{"&NetNbr", 0,  (Fl_Callback*)cb_mnuBrowseNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Lookup", 0, 0, 0, 64, 0, 0, 14, 56},
	{"On-line", 0, (Fl_Callback*)cb_LookupCall, 0, 0, 0, 0, 14, 56},
	{"Fldigi", 0,  (Fl_Callback*)cb_mnuFldigiEditor, 0, 0, 0, 0, 14, 56},
	{0},
	{0}
};

Fl_Menu_Bar *mbarMain = (Fl_Menu_Bar *)0;
Fl_Input *inpPrefix=(Fl_Input *)0;
Fl_Input *inpArea=(Fl_Input *)0;
Fl_Input *inpSuffix=(Fl_Input *)0;
Fl_Input *inpNickname=(Fl_Input *)0;
Fl_Input *inpNetNbr=(Fl_Input *)0;
Fl_Input *inpFname=(Fl_Input *)0;
Fl_Input *inpLname=(Fl_Input *)0;
Fl_Input *inpAddress=(Fl_Input *)0;
Fl_Input *inpCity=(Fl_Input *)0;
Fl_Input *inpState=(Fl_Input *)0;
Fl_Input *inpZip=(Fl_Input *)0;
Fl_Input *inpPhone=(Fl_Input *)0;
Fl_DateInput *inpBirthday=(Fl_DateInput *)0;
Fl_Input *inpSpouse=(Fl_Input *)0;
Fl_Input *inpSpBirthday=(Fl_Input *)0;
Fl_DateInput *txtLogDate=(Fl_DateInput *)0;
Fl_Button *btnFirst=(Fl_Button *)0;
Fl_Button *btnPrev=(Fl_Button *)0;
Fl_Button *btnNext=(Fl_Button *)0;
Fl_Button *btnLast=(Fl_Button *)0;
Fl_Button *btnNewSave=(Fl_Button *)0;
Fl_Button *btn2Queue=(Fl_Button *)0;
Fl_Button *btnDelete=(Fl_Button *)0;
Fl_Button *btnUpdateCancel=(Fl_Button *)0;
Fl_Button *btnClose=(Fl_Button *)0;
Fl_Output *lblFileName=(Fl_Output *)0;
Fl_Output *lblNumRecs=(Fl_Output *)0;
Fl_Input *inpCallsign=(Fl_Input *)0;
Fl_DateInput *inpPrevDate=(Fl_DateInput *)0;
Fl_Input *inpNbrLogins=(Fl_Input *)0;
Fl_Input *inpStatus=(Fl_Input *)0;
Fl_DateInput *inpJoined=(Fl_DateInput *)0;
Fl_Input *inpEmail=(Fl_Input *)0;
Fl_Input *inpComment1=(Fl_Input *)0;
Fl_Input *inpComment2=(Fl_Input *)0;

Fl_Input *inpCountry = (Fl_Input *)0;
Fl_Input *inpLocator = (Fl_Input *)0;
Fl_Output *outAzimuth = (Fl_Output *)0;
Fl_Output *outDistance = (Fl_Output *)0;

static Fl_Window *editor = (Fl_Window *)0;

void close_editor(void)
{
	if(editor)
		editor->hide();
}

Fl_Window * getEditWindow()
{
	if(!editor) {
		editor = newEditWindow(false);
	}
	return editor;
}

void cb_locator(Fl_Input *, void *)
{
	compute();
}

Fl_Window * newEditWindow(bool new_window_flag)
{

	if(editor && !new_window_flag)
		return editor;

	Fl_Window *dlgNetEdit =
	new Fl_Window(	main_window->x() + main_window->w() + 10,
				  main_window->y(),
				  535, 460, "Database Editor");
	dlgNetEdit->color(52);

	mbarMain = new Fl_Menu_Bar(0, 0, 535, 25);
	mbarMain->menu(menu_mbarMain);

	inpPrefix = new Fl_Input(10, 75, 35, 24, "Prefix");
	inpPrefix->align(FL_ALIGN_TOP_LEFT);

	inpArea = new Fl_Input(55, 75, 30, 24, "Area");
	inpArea->align(FL_ALIGN_TOP_LEFT);

	inpSuffix = new Fl_Input(95, 75, 45, 24, "Suffix");
	inpSuffix->align(FL_ALIGN_TOP_LEFT);

	inpNickname = new Fl_Input(155, 75, 150, 24, "Nickname");
	inpNickname->align(FL_ALIGN_TOP_LEFT);

	inpNetNbr = new Fl_Input(305, 75, 60, 24, "Net #");
	inpNetNbr->align(FL_ALIGN_TOP_LEFT);

	inpCountry = new Fl_Input(375, 75, 155, 24, "Country");
	inpCountry->align(FL_ALIGN_TOP_LEFT);

	inpCallsign = new Fl_Input(10, 120, 85, 24, "Callsign");
	inpCallsign->align(FL_ALIGN_TOP_LEFT);

	inpPrevDate = new Fl_DateInput(125, 120, 110, 24, "Prev Date");
	inpPrevDate->format(2);
	inpPrevDate->align(FL_ALIGN_TOP_LEFT);

	txtLogDate = new Fl_DateInput(245, 120, 110, 24, "Log Date");
	txtLogDate->format(2);
	txtLogDate->align(FL_ALIGN_TOP_LEFT);

	inpJoined = new Fl_DateInput(365, 120, 110, 24, "Joined");
	inpJoined->format(2);
	inpJoined->align(FL_ALIGN_TOP_LEFT);

	inpNbrLogins = new Fl_Input(10, 165, 80, 24, "Nbr Logins");
	inpNbrLogins->align(FL_ALIGN_TOP_LEFT);

	inpStatus = new Fl_Input(105, 165, 24, 24, "Status");
	inpStatus->align(FL_ALIGN_TOP);

	inpLocator = new Fl_Input(155, 165, 80, 24, "Locator");
	inpLocator->align(FL_ALIGN_TOP_LEFT);
	inpLocator->callback((Fl_Callback *)cb_locator);
	inpLocator->when(FL_WHEN_CHANGED);

	outAzimuth = new Fl_Output(245, 165, 80, 24, "Azimuth");
	outAzimuth->align(FL_ALIGN_TOP_LEFT);
	outAzimuth->color((Fl_Color)20);

	outDistance = new Fl_Output(335, 165, 100, 24, "Distance");
	outDistance->align(FL_ALIGN_TOP_LEFT);
	outDistance->color((Fl_Color)20);

//----------------------------------------------------------------------
	inpFname = new Fl_Input(80, 205, 105, 24, "Name:");
	inpLname = new Fl_Input(190, 205, 170, 25);
	inpAddress = new Fl_Input(80, 235, 280, 24, "Address:");
	inpCity = new Fl_Input(80, 265, 190, 24, "");
	inpState = new Fl_Input(275, 265, 35, 25);
	inpZip = new Fl_Input(320, 265, 60, 25);

	inpBirthday = new Fl_DateInput(385, 217, 120, 24, "Birthday:");
	inpBirthday->align(FL_ALIGN_TOP_LEFT);
	inpBirthday->format(1);

	inpPhone = new Fl_Input(385, 265, 135, 24, "Phone:");
	inpPhone->align(FL_ALIGN_TOP_LEFT);

	inpEmail = new Fl_Input(80, 295, 440, 24, "Email:");

	inpSpouse = new Fl_Input(80, 324, 85, 24, "Spouse:");

	inpSpBirthday = new Fl_Input(235, 324, 75, 24, "Birthday:");

	inpComment1 = new Fl_Input(80, 355, 440, 24, "Comments");

	inpComment2 = new Fl_Input(80, 385, 440, 24);

	int xpos = 45;
	int ypos = 420;
	int width = 25;
	int height = 25;

	btnFirst = new Fl_Button(xpos, ypos, width, height, "@|<");
	btnFirst->callback((Fl_Callback*)cbGoFirstRec);

	xpos += width;
	btnPrev = new Fl_Button(xpos, ypos, width, height, "@<");
	btnPrev->callback((Fl_Callback*)cbGoPrevRec);

	xpos += width;
	btnNext = new Fl_Button(xpos, ypos, width, height, "@>");
	btnNext->callback((Fl_Callback*)cbGoNextRec);

	xpos += width;
	btnLast = new Fl_Button(xpos, ypos, width, height, "@>|");
	btnLast->callback((Fl_Callback*)cbGoLastRec);

	xpos += (width + 10);
	width = 55;
	btnNewSave = new Fl_Button(xpos, ypos, width, height, "New");
	btnNewSave->callback((Fl_Callback*)cb_btnNewSave);

	xpos += (width + 10);
	btnUpdateCancel = new Fl_Button(xpos, ypos, width, height, "Update");
	btnUpdateCancel->callback((Fl_Callback*)cb_btnUpdateCancel);

	xpos += (width + 10);
	btnDelete = new Fl_Button(xpos, ypos, width, height, "Delete");
	btnDelete->callback((Fl_Callback*)cb_btnDelete);

	xpos += (width + 10);
	btn2Queue = new Fl_Button(xpos, ypos, width, height, "Queue");
	btn2Queue->callback((Fl_Callback*)cb_btn2Queue);

	xpos += (width + 30);
	btnClose = new Fl_Button(xpos, ypos, width, height, "Close");
	btnClose->callback((Fl_Callback*)cb_CloseEditor);

	lblFileName = new Fl_Output(25, 30, 410, 24);
	lblFileName->box(FL_BORDER_BOX);
	lblFileName->color(52);

	lblNumRecs = new Fl_Output(440, 30, 85, 24);
	lblNumRecs->box(FL_BORDER_BOX);
	lblNumRecs->color(53);

	dlgNetEdit->end();

	return dlgNetEdit;
}

