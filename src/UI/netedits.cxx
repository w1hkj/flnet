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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "icons.h"
#include "flinput2.h"
#include "netedits.h"
#include "netshared.h"
#include "netutils.h"
#include "lookupcall.h"
#include "date.h"
#include "calendar.h"
#include "masterdb.h"
#include "status.h"

Fl_Menu_Bar *mbarMain = (Fl_Menu_Bar *)0;

Fl_Input2 *inpPrefix = (Fl_Input2 *)0;
Fl_Input2 *inpArea = (Fl_Input2 *)0;
Fl_Input2 *inpSuffix = (Fl_Input2 *)0;
Fl_Input2 *inpNickname = (Fl_Input2 *)0;
Fl_Input2 *inpNetNbr = (Fl_Input2 *)0;
Fl_Input2 *inpFname = (Fl_Input2 *)0;
Fl_Input2 *inpLname = (Fl_Input2 *)0;
Fl_Input2 *inpAddress = (Fl_Input2 *)0;
Fl_Input2 *inpCity = (Fl_Input2 *)0;
Fl_Input2 *inpState = (Fl_Input2 *)0;
Fl_Input2 *inpZip = (Fl_Input2 *)0;
Fl_Input2 *inpPhone = (Fl_Input2 *)0;
Fl_DateInput *inpBirthday = (Fl_DateInput *)0;
Fl_Input2 *inpSpouse = (Fl_Input2 *)0;
Fl_Input2 *inpSpBirthday = (Fl_Input2 *)0;
Fl_DateInput *txtLogDate = (Fl_DateInput *)0;
Fl_Button *btnFirst = (Fl_Button *)0;
Fl_Button *btnPrev = (Fl_Button *)0;
Fl_Button *btnNext = (Fl_Button *)0;
Fl_Button *btnLast = (Fl_Button *)0;
Fl_Button *btnNewSave = (Fl_Button *)0;
Fl_Button *btn2Queue = (Fl_Button *)0;
Fl_Button *btnDelete = (Fl_Button *)0;
Fl_Button *btnUpdateCancel = (Fl_Button *)0;
Fl_Button *btnClose = (Fl_Button *)0;
Fl_Output *lblFileName = (Fl_Output *)0;
Fl_Output *lblNumRecs = (Fl_Output *)0;
Fl_Input2 *inpCallsign = (Fl_Input2 *)0;
Fl_DateInput *inpPrevDate = (Fl_DateInput *)0;
Fl_Input2 *inpNbrLogins = (Fl_Input2 *)0;
Fl_Input2 *inpStatus = (Fl_Input2 *)0;
Fl_DateInput *inpJoined = (Fl_DateInput *)0;
Fl_Input2 *inpEmail = (Fl_Input2 *)0;
Fl_Input2 *inpComment1 = (Fl_Input2 *)0;
Fl_Input2 *inpComment2 = (Fl_Input2 *)0;
Fl_Input2 *inpTraffic = (Fl_Input2 *)0;
Fl_Input2 *inpCounty = (Fl_Input2 *)0;
Fl_Input2 *inpCountry = (Fl_Input2 *)0;
Fl_Input2 *inpLocator = (Fl_Input2 *)0;

Fl_Output *out_sorted_by = (Fl_Output *)0;
Fl_Output *outAzimuth = (Fl_Output *)0;
Fl_Output *outDistance = (Fl_Output *)0;

static void cb_mnuSortByAPS(Fl_Menu_*, void*) {
	std::string p = trim(inpPrefix->value());
	std::string a = trim(inpArea->value());
	std::string s = trim(inpSuffix->value());
	SortByAPS ();
	int rnbr = getRecNbr(p, a, s);
	gotoRec(rnbr);
}

static void cb_mnuSortBySAP(Fl_Menu_*, void*) {
	std::string p = trim(inpPrefix->value());
	std::string a = trim(inpArea->value());
	std::string s = trim(inpSuffix->value());
	SortBySAP ();
	int rnbr = getRecNbr(p, a, s);
	gotoRec(rnbr);
}

static void cb_mnuSortByPAS(Fl_Menu_*, void *) {
	std::string p = trim(inpPrefix->value());
	std::string a = trim(inpArea->value());
	std::string s = trim(inpSuffix->value());
	SortByPAS ();
	int rnbr = getRecNbr(p, a, s);
	gotoRec(rnbr);
}

static void cb_mnuSortByNetNbr(Fl_Menu_*, void*) {
	std::string p = trim(inpPrefix->value());
	std::string a = trim(inpArea->value());
	std::string s = trim(inpSuffix->value());
	SortByNetNbr ();
	int rnbr = getRecNbr(p, a, s);
	gotoRec(rnbr);
}

void show_sort_order()
{
	out_sorted_by->value(
		sorted_by == PAS ? "P/A/S" :
		sorted_by == APS ? "A/P/S" :
		sorted_by == SAP ? "S/A/P" :
		"Net Nbr");
	out_sorted_by->redraw();
}

static void cb_lookup_masterdb_record(Fl_Menu_ *, void *) {
	csvRecord mrec;
	bool ok = from_masterdb(
				inpPrefix->value(),
				inpArea->value(),
				inpSuffix->value(), &mrec);
	if (ok) {

		if (progStatus.mdb_prefix)	inpPrefix->value(mrec.prefix.c_str());
		if (progStatus.mdb_area)	inpArea->value(mrec.area.c_str());
		if (progStatus.mdb_suffix)	inpSuffix->value(mrec.suffix.c_str());
		if (progStatus.mdb_callsign)	inpCallsign->value(mrec.callsign.c_str());
		if (progStatus.mdb_name)	inpNickname->value(mrec.name.c_str());
		if (progStatus.mdb_fname)	inpFname->value(mrec.fname.c_str());
		if (progStatus.mdb_lname)	inpLname->value(mrec.lname.c_str());
		if (progStatus.mdb_addr)	inpAddress->value(mrec.addr.c_str());
		if (progStatus.mdb_city)	inpCity->value(mrec.city.c_str());
		if (progStatus.mdb_state)	inpState->value(uppercase(mrec.state).c_str());
		if (progStatus.mdb_zip)		inpZip->value(mrec.zip.c_str());
		if (progStatus.mdb_phone)	inpPhone->value(mrec.phone.c_str());
		if (progStatus.mdb_birthdate)	inpBirthday->value(mrec.birthdate.c_str());
		if (progStatus.mdb_spouse)	inpSpouse->value(mrec.spouse.c_str());
		if (progStatus.mdb_sp_birth)	inpSpBirthday->value(mrec.sp_birth.c_str());
		if (progStatus.mdb_comment1)	inpComment1->value(mrec.comment1.c_str());
		if (progStatus.mdb_comment2)	inpComment2->value(mrec.comment2.c_str());
		if (progStatus.mdb_email)	inpEmail->value(mrec.email.c_str());
		if (progStatus.mdb_locator)	inpLocator->value(mrec.locator.c_str());
		if (progStatus.mdb_country)	inpCountry->value(mrec.country.c_str());
		if (progStatus.mdb_logdate)	txtLogDate->value(mrec.logdate.c_str());
		if (progStatus.mdb_prevdate)	inpPrevDate->value(mrec.prevdate.c_str());
		if (progStatus.mdb_nbrlogins)	inpNbrLogins->value(mrec.nbrlogins.c_str());
		if (progStatus.mdb_status)	inpStatus->value(mrec.status.c_str());
		if (progStatus.mdb_joined)	inpJoined->value(mrec.joined.c_str());
		if (progStatus.mdb_netnbr)	inpNetNbr->value(mrec.netnbr.c_str());
		if (progStatus.mdb_county)	inpCounty->value(mrec.county.c_str());
		if (progStatus.mdb_traffic)	inpTraffic->value(mrec.traffic.c_str());
		compute();
	}
}

void cb_add_record_to_masterdb(Fl_Menu_ *, void *) {
	csvRecord mrec;

	if (!masterdb) {
		if (!open_masterdb())
			return;
	}

	if (progStatus.mdb_prefix)		mrec.prefix = inpPrefix->value();
	if (progStatus.mdb_area)		mrec.area = inpArea->value();
	if (progStatus.mdb_suffix)		mrec.suffix = inpSuffix->value();
	if (progStatus.mdb_callsign)	mrec.callsign = inpCallsign->value();
	if (progStatus.mdb_name)		mrec.name = inpNickname->value();
	if (progStatus.mdb_fname)		mrec.fname = inpFname->value();
	if (progStatus.mdb_lname)		mrec.lname = inpLname->value();
	if (progStatus.mdb_addr)		mrec.addr = inpAddress->value();
	if (progStatus.mdb_city)		mrec.city = inpCity->value();
	if (progStatus.mdb_state)		mrec.state = uppercase(inpState->value());
	if (progStatus.mdb_zip)			mrec.zip = inpZip->value();
	if (progStatus.mdb_phone)		mrec.phone = inpPhone->value();
	if (progStatus.mdb_birthdate)	mrec.birthdate = inpBirthday->value();
	if (progStatus.mdb_spouse)		mrec.spouse = inpSpouse->value();
	if (progStatus.mdb_sp_birth)	mrec.sp_birth = inpSpBirthday->value();
	if (progStatus.mdb_comment1)	mrec.comment1 = inpComment1->value();
	if (progStatus.mdb_comment2)	mrec.comment2 = inpComment2->value();
	if (progStatus.mdb_email)		mrec.email = inpEmail->value();
	if (progStatus.mdb_locator)		mrec.locator = inpLocator->value();
	if (progStatus.mdb_country)		mrec.country = inpCountry->value();
	if (progStatus.mdb_logdate)		mrec.logdate = txtLogDate->value();
	if (progStatus.mdb_prevdate)	mrec.prevdate = inpPrevDate->value();
	if (progStatus.mdb_nbrlogins)	mrec.nbrlogins = inpNbrLogins->value();
	if (progStatus.mdb_status)		mrec.status = inpStatus->value();
	if (progStatus.mdb_joined)		mrec.joined = inpJoined->value();
	if (progStatus.mdb_netnbr)		mrec.netnbr = inpNetNbr->value();
	if (progStatus.mdb_county)		mrec.county = inpCounty->value();
	if (progStatus.mdb_traffic)		mrec.traffic = inpTraffic->value();

	int recn = binary_search_masterdb(
					0, masterdb->numrecs() - 1, 
					mrec.prefix, mrec.area, mrec.suffix);
	if (recn == -1) {
		add_to_masterdb(mrec);
		sort_master_recs();
		recn = binary_search_masterdb(
					0, masterdb->numrecs() - 1, 
					mrec.prefix, mrec.area, mrec.suffix);
	} else {
		std::string call;
		call.assign(mrec.prefix).append(mrec.area).append(mrec.suffix);
		switch (fl_choice2("%s is in master DB", "Add", "Cancel", "Replace", call.c_str())) {
			case 0: add_to_masterdb(mrec); break;
			case 1: break;
			case 2: replace_masterdb(recn, mrec); break;
		}
	}
}

Fl_Menu_Item menu_mbarMain[] = {
	{"Sea&rch", 0,  0, 0, 64, 0, 0, 14, 56},
		{"&Callsign", 0,  (Fl_Callback*)cb_mnuSearchCallsign, 0, 0, 0, 0, 14, 56},
		{"&NetNbr", 0,  (Fl_Callback*)cb_mnuSearchNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Browse", 0,  0, 0, 64, 0, 0, 14, 56},
		{"&Callsign", 0,  (Fl_Callback*)cb_mnuBrowseCallsign, 0, 0, 0, 0, 14, 56},
		{"&NetNbr", 0,  (Fl_Callback*)cb_mnuBrowseNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Lookup", 0, 0, 0, 64, 0, 0, 14, 56},
		{"&On-line", 0, (Fl_Callback*)cb_LookupCall, 0, 0, 0, 0, 14, 56},
		{"&Fldigi", 0,  (Fl_Callback*)cb_mnuFldigiEditor, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Sort", 0,  0, 0, 64, 0, 0, 14, 56},
		{"&Prefix/Area/Suffix", 0,  (Fl_Callback*)cb_mnuSortByPAS, 0, 0, 0, 0, 14, 56},
		{"&Area/Prefix/Suffix", 0,  (Fl_Callback*)cb_mnuSortByAPS, 0, 0, 0, 0, 14, 56},
		{"&Suffix/Area/Prefix", 0,  (Fl_Callback*)cb_mnuSortBySAP, 0, 0, 0, 0, 14, 56},
		{"&Net Number", 0,  (Fl_Callback*)cb_mnuSortByNetNbr, 0, 0, 0, 0, 14, 56},
	{0},
	{"&Master DB", 0, 0, 0, 64, 0, 0, 14, 56},
		{"&Lookup Call", 0,  (Fl_Callback*)cb_lookup_masterdb_record, 0, 0, 0, 0, 14, 56},
		{"&Add current record", 0, (Fl_Callback*)cb_add_record_to_masterdb, 0, 0, 0, 0, 14, 56},
	{0},
{0}
};

Fl_Double_Window *editor = (Fl_Double_Window *)0;

void close_editor(void)
{
	if (editor)
		editor->hide();
	inp_focus->take_focus();
}

void cb_locator(Fl_Input2 *, void *)
{
	compute();
}

void cb_prefix(Fl_Input2 *w, void *)
{
	static std::string str;
	str = inpPrefix->value();
	for (size_t n = 0; n < str.length(); n++) str[n] = toupper(str[n]);
	if (str.length() > 3) str.erase(3);
	inpPrefix->value(str.c_str());
	inpPrefix->redraw();
	static std::string call;
	call.assign(inpPrefix->value()).append(inpArea->value()).append(inpSuffix->value());
	inpCallsign->value(call.c_str());
	inpCallsign->redraw();
}

void cb_area(Fl_Input2 *w, void *)
{
	static std::string str;
	str = inpArea->value();
	for (size_t n = 0; n < str.length(); n++) str[n] = toupper(str[n]);
	if (str.length() > 1) str.erase(1);
	inpArea->value(str.c_str());
	inpArea->redraw();
	static std::string call;
	call.assign(inpPrefix->value()).append(inpArea->value()).append(inpSuffix->value());
	inpCallsign->value(call.c_str());
	inpCallsign->redraw();
}

void cb_suffix(Fl_Input2 *w, void *)
{
	static std::string str;
	str = inpSuffix->value();
	for (size_t n = 0; n < str.length(); n++) str[n] = toupper(str[n]);
	if (str.length() > 4) str.erase(4);
	size_t p = str.find("/");
	if (p != std::string::npos) str.erase(p);
	inpSuffix->value(str.c_str());
	inpSuffix->redraw();
	static std::string call;
	call.assign(inpPrefix->value()).append(inpArea->value()).append(inpSuffix->value());
	inpCallsign->value(call.c_str());
	inpCallsign->redraw();
}

void cbState(Fl_Input2 *w, void *)
{
	std::string ustr = uppercase(w->value());
	w->value(ustr.c_str());
}

void newEditWindow()
{
	if (editor) return;

	editor = new Fl_Double_Window(
		main_window->x() + main_window->w() + 10, main_window->y(),
		535, 460, "Database Editor");

	editor->color(52);

	mbarMain = new Fl_Menu_Bar(0, 0, 370, 25);
	mbarMain->menu(menu_mbarMain);

	out_sorted_by = new Fl_Output(460, 0, 75, 25, "Sorted by:");
	out_sorted_by->color( (Fl_Color)20);

	lblFileName = new Fl_Output(10, 30, 400, 24);
	lblFileName->box(FL_BORDER_BOX);
	lblFileName->color(52);

	lblNumRecs = new Fl_Output(415, 30, 110, 24);
	lblNumRecs->box(FL_BORDER_BOX);
	lblNumRecs->color(53);

	int y = 75;
	inpCallsign = new Fl_Input2(10, y, 110, 22, "Callsign");
	inpCallsign->tooltip("Net member Callsign");
	inpCallsign->align(FL_ALIGN_TOP_LEFT);

	inpPrefix = new Fl_Input2(125, y, 40, 22, "Prefix");
	inpPrefix->align(FL_ALIGN_TOP_LEFT);
	inpPrefix->tooltip("Callsign Prefix");
	inpPrefix->callback((Fl_Callback *)cb_prefix);

	inpArea = new Fl_Input2(170, y, 40, 22, "Area");
	inpArea->align(FL_ALIGN_TOP_LEFT);
	inpArea->tooltip("Callsign Area");
	inpArea->callback((Fl_Callback *)cb_area);

	inpSuffix = new Fl_Input2(215, y, 65, 22, "Suffix");
	inpSuffix->align(FL_ALIGN_TOP_LEFT);
	inpSuffix->tooltip("Callsign Suffix");
	inpSuffix->callback((Fl_Callback *)cb_suffix);

	inpNickname = new Fl_Input2(285, y, 145, 22, "Nickname");
	inpNickname->tooltip("Name used in QSO");
	inpNickname->align(FL_ALIGN_TOP_LEFT);

	y += 40;
	inpNetNbr = new Fl_Input2(10, y, 75, 22, "Net #");
	inpNetNbr->tooltip("Assigned net number");
	inpNetNbr->align(FL_ALIGN_TOP_LEFT);

	inpPrevDate = new Fl_DateInput(90, y, 110, 22, "Prev Date");
	inpPrevDate->format(2);
	inpPrevDate->tooltip("Date of last log-in");
	inpPrevDate->align(FL_ALIGN_TOP_LEFT);

	txtLogDate = new Fl_DateInput(205, y, 110, 22, "Log Date");
	txtLogDate->format(2);
	txtLogDate->tooltip("Current log-in date");
	txtLogDate->align(FL_ALIGN_TOP_LEFT);

	inpJoined = new Fl_DateInput(320, y, 110, 22, "Joined");
	inpJoined->format(2);
	inpJoined->tooltip("Date member joined the net");
	inpJoined->align(FL_ALIGN_TOP_LEFT);

	y += 40;
	inpNbrLogins = new Fl_Input2(10, y, 70, 22, "Nbr Logins");
	inpNbrLogins->tooltip("Number of times member has logged in to net");
	inpNbrLogins->align(FL_ALIGN_TOP_LEFT);

	inpStatus = new Fl_Input2(105, y, 22, 22, "Status");
	inpStatus->tooltip("Numeric status indicator");
	inpStatus->align(FL_ALIGN_TOP);

	inpLocator = new Fl_Input2(155, y, 70, 22, "Locator");
	inpLocator->align(FL_ALIGN_TOP_LEFT);
	inpLocator->callback((Fl_Callback *)cb_locator);
	inpLocator->tooltip("4 or 6 character Maidenhead location");
	inpLocator->when(FL_WHEN_CHANGED);

	outAzimuth = new Fl_Output(245, y, 70, 22, "Azimuth");
	outAzimuth->align(FL_ALIGN_TOP_LEFT);
	outAzimuth->tooltip("Computed azimuth, net control to member");
	outAzimuth->color((Fl_Color)20);

	outDistance = new Fl_Output(335, y, 100, 22, "Distance");
	outDistance->align(FL_ALIGN_TOP_LEFT);
	outDistance->tooltip("Computed distance, net control to member");
	outDistance->color((Fl_Color)20);

//----------------------------------------------------------------------

	y += 40;

	inpFname = new Fl_Input2(20, y, 125, 22, "First name:");
	inpFname->tooltip("first name");
	inpFname->align(FL_ALIGN_TOP_LEFT);

	inpLname = new Fl_Input2(150, y, 235, 22, "Last name");
	inpLname->tooltip("last name");
	inpLname->align(FL_ALIGN_TOP_LEFT);

	inpBirthday = new Fl_DateInput(390, y, 120, 22, "Birthday:");
	inpBirthday->tooltip("date of birth");
	inpBirthday->align(FL_ALIGN_TOP_LEFT);

	y += 26;
	inpAddress = new Fl_Input2(70, y, 265, 22, "Addr:");
	inpAddress->tooltip("Street address");

	inpPhone = new Fl_Input2(390, y, 135, 22, "Phone:");
	inpPhone->tooltip("phone #");

	y += 26;
	inpCity = new Fl_Input2(70, y, 190, 22, "City");
	inpCity->tooltip("city");

	inpState = new Fl_Input2(310, y, 50, 22, "St/Pr");
	inpState->callback((Fl_Callback *)cbState);
	inpState->when(FL_WHEN_CHANGED);
	inpState->tooltip("State");

	inpZip = new Fl_Input2(390, y, 90, 22, "ZIP");
	inpZip->tooltip("zip / postal code");

	y += 26;

	inpCounty = new Fl_Input2(70, y, 140, 22, "County:");
	inpCounty->tooltip("County or other geographic designator");

	inpCountry = new Fl_Input2(310, y, 140, 22, "Country:");
	inpCountry->tooltip("country");

	y += 26;
	inpEmail = new Fl_Input2(70, y, 440, 22, "Email:");

	y += 26;
	inpSpouse = new Fl_Input2(70, y, 85, 22, "Spouse:");

	inpSpBirthday = new Fl_Input2(275, y, 75, 22, "Spouse Bday:");

	y += 26;
	inpTraffic = new Fl_Input2(70, y, 440, 22, "Traffic:");
	inpTraffic->tooltip("traffic net, passed etc");

	y += 26;
	inpComment1 = new Fl_Input2(70, y, 440, 22, "Info:");
	y += 26;
	inpComment2 = new Fl_Input2(70, y, 440, 24);

	int xpos = 45;
	int ypos = editor->h() - 30;
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

	editor->end();

	return;
}

