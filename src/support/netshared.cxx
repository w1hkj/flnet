// =====================================================================
//
// netshared.cxx
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


#include <string>
#include <sstream>
#include <climits>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/filename.H>

#include "netshared.h"
#include "my_UI.h"
#include "loglist.h"
#include "lookupcall.h"
#include "netsupport.h"
#include "config.h"
#include "net_config.h"
#include "xml_io.h"
#include "csvdb.h"
#include "status.h"
#include "icons.h"

csvdb netdb;

extern loglist callinlist;

int  sorted_by = 0;

int  binary_search_SAP(int l, int r, std::string &p, std::string &a, std::string &s);
int  binary_search_netnbr(int l, int r, int x);

short	rc;
int		fc, nrecs;

index_struct *indexed_list = NULL;
static int brwstabs[] = { 80, 0 };

State editState = UPDATE;

size_t		currec;
size_t		list_index;
size_t		AddedRecNbr;

std::string sSimpleName;
char szDispName[200];

Fl_Window	*NetNbrSearch = NULL,
*NetNbrBrowse = NULL,
*CallsignSearch = NULL,
*CallsignBrowse = NULL;

void close_misc_dialogs(void)
{
	if(NetNbrSearch)   NetNbrSearch->hide();
	if(NetNbrBrowse)   NetNbrBrowse->hide();
	if(CallsignSearch) CallsignSearch->hide();
	if(CallsignBrowse) CallsignBrowse->hide();
}

std::string uppercase( std::string s)
{
	static std::string ucase;
	ucase.clear();
	ucase.assign(s);
	for (size_t i = 0; i < s.length(); i++)
		ucase[i] = toupper(ucase[i]);
	return ucase;
}

std::string trim(std::string s)
{
	static std::string trimmed;
	trimmed.clear();
	trimmed.assign(s);
	while (trimmed.length() && trimmed[0] == ' ') trimmed.erase(0);
	while (trimmed.length() && trimmed[trimmed.length()-1] == ' ')
		trimmed.erase(trimmed.length() - 1);
	return trimmed;
}


void gotoRec (long L)
{
	currec = L;
	dispRec ();
}

void showState ()
{
	Fl_Menu_Item *item1 = NULL, *item2 = NULL, *item3 = NULL;
	item1 = (Fl_Menu_Item*)mbarMain->find_item("Sea&rch");
	item2 = (Fl_Menu_Item*)mbarMain->find_item("&Browse");
	item3 = (Fl_Menu_Item*)mbarMain->find_item("&Sort");

	if ( item1 != NULL ) item1->activate();
	if ( item2 != NULL ) item2->activate();
	if ( item3 != NULL ) item3->activate();

	switch (editState) {
		case UPDATE:
			btnNewSave->label("New");
			btnUpdateCancel->label ("Update");
			btnNewSave->show ();
			btnUpdateCancel->show();
			btnDelete->show ();
			btnFirst->show ();
			btnPrev->show ();
			btnNext->show ();
			btnLast->show ();
			btn2Queue->show ();
			btnClose->show();
			break;

		case NEW:
			btnNewSave->label ("Save");
			btnUpdateCancel->label ("Cancel");
			btnNewSave->show ();
			btnDelete->hide ();
			btnFirst->hide ();
			btnPrev->hide ();
			btnNext->hide ();
			btnLast->hide ();
			btn2Queue->hide ();
			btnClose->hide();
			break;

		case ADD:
			btnNewSave->label ("Save");
			btnUpdateCancel->label ("Cancel");
			btnNewSave->show ();
			btnDelete->hide ();
			btnFirst->hide ();
			btnPrev->hide ();
			btnNext->hide ();
			btnLast->hide ();
			btn2Queue->hide ();
			btnClose->hide();
			break;

		case MODIFY:
			btnNewSave->hide ();
			btnDelete->hide ();
			btnFirst->hide ();
			btnPrev->hide ();
			btnNext->hide ();
			btnLast->hide ();
			btn2Queue->hide ();
			btnUpdateCancel->label ("Update");
			btnUpdateCancel->show();
			btnClose->show();

			if ( item1 != NULL ) item1->deactivate();
			if ( item2 != NULL ) item2->deactivate();
			if ( item3 != NULL ) item3->deactivate();

			break;
	}
	editor->redraw();
}

void toggleState()
{
	if (editState == NEW || editState == ADD) {
		editState = UPDATE;
		showState ();
	} else {
		showState ();
	}
}


// execute after database record modification of any kind
void refresh_logins()
{
	if (!callinlist.numlist()) return;

	std::string p, a, s;
	SortBySAP();
	int rn;
	for (int i = 0; i < callinlist.numlist(); i++) {
		p = callinlist.prefix(i);
		a = callinlist.area(i);
		s = callinlist.suffix(i);
		rn = binary_search_SAP(0, netdb.numrecs(), p, a ,s);
		callinlist.recN(i, rn);
	}
	SortByPreferred();
	while (WhoIsUp >= callinlist.numlist()) WhoIsUp--;
	if (WhoIsUp < 0) WhoIsUp = 0;
	updateCallins (false);
}

extern Fl_Input2 *inpCallsign;

void AddNewRecord (const char *prefix, const char *area, const char *suffix)
{
	clearEditForm ();
	inpPrefix->value (prefix);
	inpArea->value (area);
	inpSuffix->value (suffix);
	std::string tmp;
	tmp.assign(trim(prefix)).append(trim(area)).append(trim(suffix));
	inpCallsign->value (tmp.c_str());
	inpPrefix->take_focus ();
	editState = ADD;
	showState ();
}

void ModifyRecord (long N)
{
	editState = MODIFY;
	showState ();
	gotoRec (N);
}

int get_cnt = 0;
void getindexed_list()
{
	if (indexed_list) {
		delete [] indexed_list;
		indexed_list = NULL;
	}

	if (!netdb.numrecs()) {
		update_select_label();
		return;
	}

	update_select_label();

	currec = netdb.recnbr();

	csvRecord rec;
	indexed_list = new index_struct[netdb.numrecs()];

	for (size_t n = 0; n < netdb.numrecs(); n++) {
		netdb.get(n, rec);
		indexed_list[n].recN = n;
		memset(indexed_list[n].netnbr, 0, 5);
		memset(indexed_list[n].prefix, 0, 4);
		memset(indexed_list[n].area, 0, 2);
		memset(indexed_list[n].suffix, 0, 5);
		strncpy (indexed_list[n].netnbr, rec.netnbr.c_str(), 4);
		strncpy (indexed_list[n].prefix, rec.prefix.c_str(), 3);
		strncpy (indexed_list[n].area, rec.area.c_str(), 1);
		strncpy (indexed_list[n].suffix, rec.suffix.c_str(), 4);

		while (strlen(indexed_list[n].suffix) < 4)
			strcat (indexed_list[n].suffix, " ");
	}
	netdb.get(currec, rec);
}

int NetNbrCompare (const void *p1, const void *p2)
{
	index_struct *s1 = (index_struct *)p1;
	index_struct *s2 = (index_struct *)p2;
	int n1 = atoi(s1->netnbr);
	int n2 = atoi(s2->netnbr);
	if (n1 == 0 && n2 == 0) {
		int cmp;
		if ((cmp = strcmp(s1->prefix, s2->prefix)) != 0) return cmp;
		if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
		return strcmp(s1->suffix, s2->suffix);
	}
	if (n1 == 0) return 1;
	if (n2 == 0) return -1;
	if (n1 < n2) return -1;
	return 1;
}

int SAPcompare (const void *p1, const void *p2)
{
	index_struct *s1 = (index_struct *)p1;
	index_struct *s2 = (index_struct *)p2;
	int cmp;
	if ((cmp = strcmp(s1->suffix, s2->suffix)) != 0) return cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	return strcmp(s1->prefix, s2->prefix);
}

int APScompare (const void *p1, const void *p2)
{
	index_struct *s1 = (index_struct *)p1;
	index_struct *s2 = (index_struct *)p2;
	int cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	if ((cmp = strcmp(s1->prefix, s2->prefix)) != 0) return cmp;
	return strcmp(s1->suffix, s2->suffix);
}

int PAScompare (const void *p1, const void *p2)
{
	index_struct *s1 = (index_struct *)p1;
	index_struct *s2 = (index_struct *)p2;
	int cmp;
	if ((cmp = strcmp(s1->prefix, s2->prefix)) != 0) return cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	return strcmp(s1->suffix, s2->suffix);
}

void SortByPAS()
{
	sorted_by = PAS;
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), PAScompare);
	show_sort_order();
}

void SortByAPS()
{
	sorted_by = APS;
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), APScompare);
	show_sort_order();
}

void SortBySAP()
{
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), SAPcompare);
	sorted_by = SAP;
	show_sort_order();
}

void SortByNetNbr()
{
	sorted_by = NETNBR;
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), NetNbrCompare);
	show_sort_order();
}

void reSort ()
{
	if (!indexed_list || !netdb.numrecs()) return;
	int found = -1;

	std::string p = trim(inpPrefix->value());
	std::string a = trim(inpArea->value());
	std::string s = trim(inpSuffix->value());

	switch (sorted_by) {
		case PAS : 
		case NETNBR :
			sorted_by = PAS;
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), PAScompare);
			found = binary_search_PAS( 0, netdb.numrecs() - 1, p, a, s);
			break;
		case APS :
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), APScompare);
			found = binary_search_APS( 0, netdb.numrecs() - 1, p, a, s);
			break;
		case SAP :
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), SAPcompare);
			found = binary_search_SAP( 0, netdb.numrecs() - 1, p, a, s);
			break;
	}
	show_sort_order ();
	if (found > -1)
		gotoRec(found);
}

void SortByPreferred()
{
	switch (progStatus.preferred_sort_order) {
		case PAS : SortByPAS (); break;
		case APS : SortByAPS (); break;
		case SAP : SortBySAP (); break;
		case NETNBR : SortByNetNbr (); break;
	}
}

void SortBy (int by)
{
	switch (by) {
		case PAS : SortByPAS (); break;
		case APS : SortByAPS (); break;
		case SAP : SortBySAP (); break;
		case NETNBR : SortByNetNbr (); break;
	}
}

void closeDB()
{
	netdb.save();

	std::string cfg_filename = home_dir;
	cfg_filename.append("flnet.cfg");
	FILE *cfg_file = fopen(cfg_filename.c_str(),"w");
	fprintf(cfg_file, "%s\n", selected_file.c_str());
	fclose(cfg_file);

}

void update_select_label()
{
	static char dispname[31];
	snprintf(dispname, sizeof(dispname), "%s (%lu)", sSimpleName.c_str(), (unsigned long)netdb.numrecs());
	while (strlen(dispname) < 30) strcat(dispname, " ");
	box_db_select->label(dispname);
	box_db_select->redraw_label();
}

void openDB(std::string fname)
{
	sSimpleName = fl_filename_name(fname.c_str());
	netdb.filename(fname.c_str());
	if (netdb.load() != 0) {
		fl_message("Not an flnet csv file");
		exit(0);
	}
	callinlist.AutoPriority (progStatus.chAuto);
	if (netdb.numrecs()) {
		callinlist.setPri_1 (progStatus.chP1[0]);
		callinlist.setPri_2 (progStatus.chP2[0]);
		callinlist.setPri_3 (progStatus.chP3[0]);
		callinlist.sort(loglist::BYPRIORITY);
		getindexed_list();
	} else {
		callinlist.sort(loglist::BYDATETIME);
		update_select_label();
	}
}

void dispRec ()
{
	static char fname[200];
	snprintf(fname, sizeof(fname), "File: %s", sSimpleName.c_str());
	lblFileName->value (fname);

	static char recs[50];
	snprintf(recs, sizeof(recs), "Recs: %d", (int)netdb.numrecs());
	lblNumRecs->value (recs);

	csvRecord rec;

	netdb.get(currec, rec);

	inpPrefix->value (trim (rec.prefix).c_str());
	inpArea->value (trim (rec.area).c_str());
	inpSuffix->value (trim (rec.suffix).c_str());
	inpNickname->value (trim (rec.name).c_str());
	inpNetNbr->value (trim (rec.netnbr).c_str());
	txtLogDate->value( rec.logdate.c_str());
	inpFname->value (trim (rec.fname).c_str());
	inpLname->value (trim (rec.lname).c_str());
	inpAddress->value (trim (rec.addr).c_str());
	inpCity->value (trim (rec.city).c_str());
	inpState->value (trim (rec.state).c_str());
	inpZip->value (trim (rec.zip).c_str());
	inpPhone->value (trim (rec.phone).c_str());
	inpSpouse->value (trim (rec.spouse).c_str());
	inpSpBirthday->value (trim (rec.sp_birth).c_str());
	inpBirthday->value(rec.birthdate.c_str());

	inpCallsign->value (trim (rec.callsign).c_str());
	inpNbrLogins->value (trim (rec.nbrlogins).c_str());
	inpStatus->value (trim (rec.status).c_str());
	inpJoined->value (trim (rec.joined).c_str());
	inpComment1->value (trim (rec.comment1).c_str());
	inpComment2->value (trim (rec.comment2).c_str());
	inpEmail->value (trim (rec.email).c_str());
	inpPrevDate->value (trim (rec.prevdate).c_str());

	inpLocator->value (trim (rec.locator).c_str());
	inpCountry->value (trim (rec.country).c_str());

	outAzimuth->value("");
	outDistance->value("");
	compute();
}

void clearEditForm ()
{
	inpPrefix->value ("");
	inpArea->value ("");
	inpSuffix->value ("");
	inpNickname->value ("");
	inpNetNbr->value ("");
	txtLogDate->value ("");
	inpFname->value ("");
	inpLname->value ("");
	inpAddress->value ("");
	inpCity->value ("");
	inpState->value ("");
	inpZip->value ("");
	inpPhone->value ("");
	inpBirthday->value ("");
	inpSpouse->value ("");
	inpSpBirthday->value ("");
	inpCallsign->value ("");
	inpNbrLogins->value ("");
	inpStatus->value ("");
	inpJoined->value ("");
	inpComment1->value ("");
	inpComment2->value ("");
	inpPrevDate->value ("");
	inpEmail->value ("");
	inpCountry->value ("");
	inpLocator->value ("");
	outAzimuth->value ("");
	outDistance->value ("");
}

void setFields (csvRecord &rec)
{
	rec.prefix = uppercase (inpPrefix->value ());
	rec.area = inpArea->value ();
	rec.suffix =	uppercase (inpSuffix->value ());
	rec.name = inpNickname->value ();
	rec.netnbr = inpNetNbr->value ();
	rec.logdate = txtLogDate->value();
	rec.fname = inpFname->value ();
	rec.lname = inpLname->value ();
	rec.addr	= inpAddress->value ();
	rec.city	= inpCity->value ();
	rec.state = inpState->value ();
	rec.zip	 = inpZip->value ();
	rec.phone = inpPhone->value ();
	rec.birthdate = inpBirthday->value ();
	rec.spouse = inpSpouse->value ();
	rec.sp_birth = inpSpBirthday->value ();
	rec.callsign = inpCallsign->value();
	rec.nbrlogins = inpNbrLogins->value ();
	rec.status = inpStatus->value ();
	rec.joined = inpJoined->value();
	rec.comment1 = inpComment1->value ();
	rec.comment2 = inpComment2->value ();
	rec.email = inpEmail->value ();
	rec.prevdate = inpPrevDate->value();
	rec.locator = inpLocator->value();
	rec.country = inpCountry->value();

}

void saveCurRecord ()
{
	csvRecord rec;
	setFields (rec);
	netdb.put(currec, rec);
	getindexed_list ();
	dispRec ();
}

void appendNewRecord ()
{
	csvRecord rec;
	setFields (rec);
	netdb.add(rec);
	getindexed_list ();
}

void appendNewRecord (csvRecord &rec)
{
	setFields (rec);
	netdb.add(rec);
	getindexed_list ();
}


void add_fldigi_record(void)
{
	std::string prefix, area, suffix;

	if (!fldigi_online) {
		fl_alert ("FLDIGI<->FLNET XMLRPC Commmunication failure!");
		return;
	}

	struct callsign_data *data = update_flnet_calldata();

	if (!data) {
		fl_alert ("XMLRPC Internal Data transfer failure!");
		return;
	}

	if (split_call(data->callsign, prefix, area, suffix) == false) {
		delete data;
		return;
	}
	data->callsign.assign(prefix).append(area).append(suffix);

	int recn = netdb.find_callsign(data->callsign);

	if (recn >= 0) {	// existing record in data base
		add_to_callins(recn);
		delete data;
		return;
	}

// not in data base
// add new record with data received from fldigi

	csvRecord rec;
	rec.prefix.assign (prefix);
	rec.area.assign (area);
	rec.suffix.assign (suffix);
	rec.name.assign (data->name);
	rec.fname.assign (data->name);
	rec.city.assign (data->qth);
	rec.state.assign (data->state);
	rec.callsign.assign (data->callsign);
	rec.netnbr.clear ();
	rec.logdate.clear ();
	rec.lname.clear ();
	rec.addr.clear ();
	rec.zip.clear ();
	rec.phone.clear ();
	rec.birthdate.clear ();
	rec.spouse.clear ();
	rec.sp_birth.clear ();
	rec.nbrlogins.clear ();
	rec.status.clear ();
	rec.joined.clear ();
	rec.comment1.clear ();
	rec.comment2.clear ();
	rec.email.clear ();
	rec.prevdate.clear ();
	rec.country.clear ();
	rec.locator.clear ();

	netdb.add (rec);

	getindexed_list ();

	recn = netdb.find_callsign(data->callsign);
	if (recn >= 0) {
		add_to_callins(recn);
	}

	delete data;

}

bool split_call(std::string src, std::string &pre, std::string &area, std::string &post)
{

	if(src.empty()) {
		return false;
	}

	size_t index = 0;

	index = src.rfind("/");
	if (index != std::string::npos)
		src.erase(index);
	index = src.find("/");
	if (index != std::string::npos)
		src.erase(0, index + 1);

	for (index = 0; index < src.length(); index++)
		if (!isdigit(src[index]) && !isalpha(src[index])) {
			return false;
		}

	for (index = 0; index < src.length(); index++)
		src[index] = toupper(src[index]);

	if (src.length() == 3) {
		pre = src[0];
		area = src[1];
		post = src[2];
		return true;
	}

	if (src.length() == 4) {
		if (isdigit(src[1])) { // PNSS
			pre = src[0];
			area = src[1];
			post = src.substr(2);
			return true;
		}
		if (isdigit(src[2])) { // PPNS
			pre = src.substr(0, 2);
			area = src[2];
			post = src[3];
			return true;
		}
		return false;
	}

	if ((src.length() == 5) && isdigit(src[1])) { // PNSSS
		pre = src[0];
		area = src[1];
		post = src.substr(2);
			return true;
	}

	if ((src.length() == 5) && isdigit(src[2])) { // PPNSS
		pre = src.substr(0, 2);
		area = src[2];
		post = src.substr(3);
		return true;
	}

	if ((src.length() == 6) && isdigit(src[2])) { // PPNSSS
		pre = src.substr(0,2);
		area = src[2];
		post = src.substr(3);
		return true;
	}

	if ((src.length() == 6) && isdigit(src[3])) { // PPPNSS
		pre = src.substr(0,3);
		area = src[3];
		post = src.substr(4);
		return true;
	}
	if ((src.length() == 7) && isdigit(src[2])) { // PPNSSSS
		pre = src.substr(0,2);
		area = src[2];
		post = src.substr(3);
		return true;
	}

	if ((src.length() == 7) && isdigit(src[3])) { // PPPNSSS
		pre = src.substr(0,3);
		area = src[3];
		post = src.substr(4);
		return true;
	}

	return false;
}

void cb_mnuFldigiEditor(Fl_Menu_*, void*)
{
	add_fldigi_record();
}

void cb_F12(int WhoIsUp)
{
	if (!editor) return;

	clearEditForm ();

	std::string prefix = trim(callinlist.prefix (WhoIsUp));
	std::string area = trim(callinlist.area (WhoIsUp));
	std::string suffix = trim(callinlist.suffix (WhoIsUp));

// do not allow empty database entry!
	if (prefix.empty() || area.empty() || suffix.empty()) {
		fl_alert2("No check-in selected");
		return;
	}
	std::string tmp;
	tmp.assign(trim(prefix)).append(trim(area)).append(trim(suffix));

	SortBySAP ();
	int rnbr = binary_search_SAP(0, netdb.numrecs(), prefix, area, suffix);

	if (rnbr < 0) {
		char date[32];
		time_t rawtime = 0;
		struct tm * timeinfo = 0;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		memset(date, 0, sizeof(date));
		snprintf(date, sizeof(date)-1, "%04d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

		csvRecord rec;
		rec.prefix.assign(prefix);
		rec.area.assign(area);
		rec.suffix.assign(suffix);
		rec.name.clear();
		rec.fname.clear();
		rec.city.clear();
		rec.state.clear();
		rec.callsign.assign(trim(prefix)).append(trim(area)).append(trim(suffix));
		rec.netnbr.clear();
		rec.logdate.clear();
		rec.lname.clear();
		rec.addr.clear();
		rec.zip.clear();
		rec.phone.clear();
		rec.birthdate.clear();
		rec.spouse.clear();
		rec.sp_birth.clear();
		rec.nbrlogins.clear();
		rec.status.clear();
		rec.joined.clear();
		rec.comment1.clear();
		rec.comment2.clear();
		rec.email.clear();
		rec.prevdate.clear();
		rec.country.clear();
		rec.locator.clear();

		netdb.add(rec);

		getindexed_list ();

		SortBySAP();
		rnbr = binary_search_SAP(0, netdb.numrecs(), prefix, area, suffix);
	}
	SortByPreferred ();
	show_sort_order ();
	ModifyRecord(rnbr);

	editor->show();
	inpNickname->take_focus();

}

void cb_ShiftF12(void)
{
	add_fldigi_record();
	return;
}

int  getRecNbr(std::string p, std::string a, std::string s)
{
	int rnbr = -1;
	switch (sorted_by) {
		case NETNBR :
			qsort (&(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), NetNbrCompare);
			rnbr = -1;
			for (size_t i = 0; i < netdb.numrecs(); i++) {
				if ((p == trim(indexed_list[i].prefix)) && 
					(a == trim(indexed_list[i].area)) &&
					(s == trim(indexed_list[i].suffix))) {
					list_index = i;
					rnbr = indexed_list[i].recN;
					break;
				}
			}
			break;
		case PAS : 
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), PAScompare);
			rnbr = binary_search_PAS( 0, netdb.numrecs() - 1, p, a, s);
			break;
		case APS :
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), APScompare);
			rnbr = binary_search_APS( 0, netdb.numrecs() - 1, p, a, s);
			break;
		case SAP :
			qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), SAPcompare);
			rnbr = binary_search_SAP( 0, netdb.numrecs() - 1, p, a, s);
			break;
	}
	return rnbr;
}

void cbEditor ()
{
	if (!editor) newEditWindow();

	std::string p;
	std::string a;
	std::string s;

	if (WhoIsUp >= 0) {
		p = trim(callinlist.prefix (WhoIsUp));
		a = trim(callinlist.area (WhoIsUp));
		s = trim(callinlist.suffix (WhoIsUp));
	}

	if (p.empty() || a.empty() || s.empty()) {
		show_sort_order();
		clearEditForm ();
		editor->show();
		cbGoFirstRec (NULL, NULL);
		inpNickname->take_focus();
		editState = UPDATE;
		showState ();
		return;
	}

	int rnbr = getRecNbr(p, a, s);
	show_sort_order ();
	if (rnbr > -1)
		gotoRec(rnbr);
	else
		gotoRec(0);

	inpNickname->take_focus();
	editState = UPDATE;
	showState ();
	editor->show();
	return;
}

void cbCloseEditor ()
{
	if (!editor) return;

	editor->hide ();
	getindexed_list ();
	refresh_logins ();
	updateCallins ();
	inp_focus->take_focus();
}

void cb_btnCancelCallsignSearch(Fl_Button*, void*)
{
	CallsignBrowse->hide();
}

void browserCallsignSearch()
{
	int selrec = brwsCallsign->value();
	CallsignBrowse->hide();
	if (selrec != 0) {
		list_index = selrec - 1;
		gotoRec ( indexed_list[list_index].recN);
	}
}

void cb_CallsignSearch(Fl_Select_Browser *, void *)
{
	int e = Fl::event_key();
	if (e == FL_Enter || e == FL_KP_Enter)
		browserCallsignSearch();
}

void cb_OkCallsignSearch(Fl_Button*, void*)
{
	browserCallsignSearch();
}

void cb_mnuBrowseCallsign (Fl_Menu_*, void*)
{
	char brwsLine[30];
	if (!CallsignBrowse) CallsignBrowse = winCallsignBrowse();
	getindexed_list();
	SortByPAS();
	out_sorted_by->value("P/A/S");
	out_sorted_by->redraw();
	brwsCallsign->column_widths(brwstabs);
	brwsCallsign->clear ();
	for (size_t i = 0; i < netdb.numrecs(); i++) {
		strcpy(brwsLine, indexed_list[i].netnbr);
		strcat(brwsLine, "\t");
		strcat(brwsLine, indexed_list[i].prefix);
		strcat(brwsLine, indexed_list[i].area);
		strcat(brwsLine, indexed_list[i].suffix);
		brwsCallsign->add (brwsLine);
	}
	brwsCallsign->topline(1);
	brwsCallsign->select(1);
	CallsignBrowse->show();
}

void cb_btnCancelNetNbrSearch(Fl_Button*, void*)
{
	NetNbrBrowse->hide();
}

void browserNetNbrSearch()
{
	int selrec = brwsNetNbr->value();
	NetNbrBrowse->hide();
	if (selrec != 0) {
		list_index = selrec - 1;
		gotoRec (indexed_list[list_index].recN);
	}
}

void cb_NetNbrSearch(Fl_Select_Browser *, void *)
{
	int e = Fl::event_key();
	if (e == FL_Enter || e == FL_KP_Enter)
		browserNetNbrSearch();
}

void cb_OkNetNbrSearch(Fl_Button*, void*)
{
	browserNetNbrSearch();
}

void cb_mnuBrowseNetNbr (Fl_Menu_*, void*)
{
	char brwsLine[30];
	if (!NetNbrBrowse) NetNbrBrowse = winNetNbrBrowse();
	getindexed_list();
	SortByNetNbr();
	out_sorted_by->value("Net Nbr");
	out_sorted_by->redraw();
	brwsNetNbr->clear ();
	brwsNetNbr->column_widths(brwstabs);
	for (size_t i = 0; i < netdb.numrecs(); i++) {
		strcpy(brwsLine, indexed_list[i].netnbr);
		strcat(brwsLine, "\t");
		strcat(brwsLine, indexed_list[i].prefix);
		strcat(brwsLine, indexed_list[i].area);
		strcat(brwsLine, indexed_list[i].suffix);
		brwsNetNbr->add (brwsLine);
	}
	brwsNetNbr->select(1);
	NetNbrBrowse->show();
}

void cb_btnSearchCancel(Fl_Button *b, void *d)
{
	CallsignSearch->hide ();
}

// l = left element in search interval
// r = right element in search interval
// p = search prefix
// a = search area
// s = search suffix
// indexed_list must be sorted in SAP order
int  binary_search_SAP(int l, int r, std::string &p, std::string &a, std::string &s)
{
	if (!indexed_list | (netdb.numrecs() == 0)) return -1;
	std::string p2, a2, s2;
	if (r >= l) {
		int mid = l + (r - l) / 2;
		p2 = trim(indexed_list[mid].prefix);
		a2 = trim(indexed_list[mid].area);
		s2 = trim(indexed_list[mid].suffix);
		// compare suffix
		if (s2 > s) return binary_search_SAP(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare area
		if (a2 > a) return binary_search_SAP(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare prefix
		if (p2 > p) return binary_search_SAP(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_SAP(mid + 1, r, p, a, s);
		list_index = mid;
		return indexed_list[mid].recN;
	}
	return -1;
}

// indexed_list must be sorted in PAS order
int  binary_search_PAS(int l, int r, std::string &p, std::string &a, std::string &s)
{
	if (!indexed_list | (netdb.numrecs() == 0)) return -1;
	std::string p2, a2, s2;
	if (r >= l) {
		int mid = l + (r - l) / 2;
		p2 = trim(indexed_list[mid].prefix);
		a2 = trim(indexed_list[mid].area);
		s2 = trim(indexed_list[mid].suffix);
		// compare prefix
		if (p2 > p) return binary_search_PAS(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_PAS(mid + 1, r, p, a, s);
		// compare area
		if (a2 > a) return binary_search_PAS(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_PAS(mid + 1, r, p, a, s);
		// compare suffix
		if (s2 > s) return binary_search_PAS(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_PAS(mid + 1, r, p, a, s);
		list_index = mid;
		return indexed_list[mid].recN;
	}
	return -1;
}

// indexed_list must be sorted in APS order
int  binary_search_APS(int l, int r, std::string &p, std::string &a, std::string &s)
{
	if (!indexed_list | (netdb.numrecs() == 0)) return -1;
	std::string p2, a2, s2;
	if (r >= l) {
		int mid = l + (r - l) / 2;
		p2 = trim(indexed_list[mid].prefix);
		a2 = trim(indexed_list[mid].area);
		s2 = trim(indexed_list[mid].suffix);
		// compare area
		if (a2 > a) return binary_search_APS(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_APS(mid + 1, r, p, a, s);
		// compare prefix
		if (p2 > p) return binary_search_APS(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_APS(mid + 1, r, p, a, s);
		// compare suffix
		if (s2 > s) return binary_search_APS(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_APS(mid + 1, r, p, a, s);
		list_index = mid;
		return indexed_list[mid].recN;
	}
	return -1;
}

void CallsignSearchOK()
{
	long found = -1;
	if (!indexed_list) return;

	CallsignSearch->hide ();

	std::string call = trim(uppercase(sSrchCall->value()));

	if (call.empty()) return;

	size_t cptr = std::string::npos;
	for (int n = call.length() - 1; n >= 0; n--) {
		if (isdigit(call[n])) {
			cptr = n;
			break;
		}
	}

	if (cptr == std::string::npos) return;

	std::string p = call.substr(0, cptr);
	std::string a = call.substr(cptr, 1);
	std::string s = call.substr(cptr + 1);

	if (p.empty() || a.empty() || s.empty() ) return;

	found = getRecNbr(p, a, s);

	gotoRec(found);
}

void cb_SrchCall(Fl_Input2 *c, void *d)
{
	CallsignSearchOK();
}

void cb_btnSearchOK(Fl_Return_Button *b, void *d)
{
	CallsignSearchOK();
}

void cb_mnuSearchCallsign (Fl_Menu_ *m, void *d)
{
	if (!CallsignSearch)
		CallsignSearch = newSearchCallsignDialog();
	sSrchCall->value("");
	sSrchCall->take_focus();
	CallsignSearch->show();
}

void cb_btnSearchNetNbrCancel (Fl_Button *b, void *d)
{
	NetNbrSearch->hide ();
}

// l = left element in search interval
// r = right element in search interval
// x = search value
// index_list sorted by netnbr
int  binary_search_netnbr(int l, int r, int x)
{
	if (!indexed_list | (netdb.numrecs() == 0)) return -1;
	int snbr;
	if (r >= l) {
		int mid = l + (r - l) / 2;
		if (strlen(indexed_list[mid].netnbr) == 0)
			snbr = INT_MAX;
		else
			snbr = atol(indexed_list[mid].netnbr);
		if (snbr == x) {
			list_index = mid;
			return mid;
		}
		if (snbr > x) return binary_search_netnbr(l, mid - 1, x);
		return binary_search_netnbr(mid + 1, r, x);
	}
	return -1;
}

void SearchNetNbrOK()
{
	if (!indexed_list) return;
	int srchnbr = atol(sSrchNetNbr->value());
	int found = -1;
	NetNbrSearch->hide ();

	SortByNetNbr ();
	out_sorted_by->value("Net Nbr");
	out_sorted_by->redraw();

	found = binary_search_netnbr(0, netdb.numrecs() - 1, srchnbr);
	if (found == -1) return;

	gotoRec(indexed_list[found].recN);
}

void cb_SrchNetNbr(Fl_Input2 *c, void *d)
{
	SearchNetNbrOK();
}

void cb_btnSearchNetNbrOK (Fl_Return_Button *b, void *d)
{
	SearchNetNbrOK();
}

void cb_mnuSearchNetNbr (Fl_Menu_ *m, void *d)
{
	if (!NetNbrSearch)
		NetNbrSearch = newSearchNetNbrDialog ();
	sSrchNetNbr->value("");
	NetNbrSearch->show ();
}

void cb_btn2Queue(Fl_Button *b, void *d)
{
	std::string call = trim(inpPrefix->value());
	call.append(trim(inpArea->value()));
	call.append(trim(inpSuffix->value()));
	long rn = netdb.find_callsign(call);
	add_to_callins(rn);
}

void cb_btnUpdateCancel(Fl_Button *b, void *d)
{
	if (!editor) newEditWindow();

	switch (editState) {
		case ADD :
			toggleState ();
			break;
		case NEW :
			clearEditForm ();
			dispRec ();
			toggleState ();
			break;
		case MODIFY :
			callinlist.modify (WhoIsUp, currec,
						   inpPrefix->value(),
						   inpArea->value (),
						   inpSuffix->value (),
						   inpNickname->value ());
			saveCurRecord ();
			getindexed_list ();
			toggleState ();
			refresh_logins ();
			updateCallins ();
			reSort ();
			break;
		case UPDATE :
			if (netdb.numrecs() > 0) {
				int returnto = currec;
				saveCurRecord ();
				getindexed_list ();
				csvRecord rec;
				int recN;
				for (int rN = 0; rN < callinlist.numlist(); rN++) {
					recN = callinlist.recN(rN);
					netdb.get(recN, rec);
					callinlist.modify (rN, recN, rec.prefix.c_str(), rec.area.c_str(), rec.suffix.c_str(), rec.name.c_str() );
				}
				refresh_logins();
				updateCallins ();
				gotoRec(returnto);
				reSort ();
			}
			break;
		default :
			break;
	}
}

void cb_btnDelete(Fl_Button *b, void *d)
{
	if (fl_choice("Confirm Delete","cancel","OK", NULL) == 1) {
		netdb.erase (currec);
		getindexed_list ();
		refresh_logins ();
		cbGoFirstRec (NULL,NULL);
		update_select_label();
	}
}

void cbGoFirstRec(Fl_Button *b, void *d)
{
	if (!indexed_list) {
		clearEditForm ();
		lblNumRecs->value ("Recs: 0");
		return;
	}
	list_index = 0;
	gotoRec (indexed_list[list_index].recN);
}

void cbGoPrevRec(Fl_Button *b, void *d)
{
	if (!indexed_list || list_index == 0) return;
	list_index--;
	if (list_index < 0) list_index = 0;
	int rn = indexed_list[list_index].recN;
	gotoRec (rn);
}

void cbGoNextRec(Fl_Button *b, void *d)
{
	if (!indexed_list || list_index == (netdb.numrecs() - 1)) return;
	list_index++;
	if (list_index >= netdb.numrecs()) list_index = netdb.numrecs() - 1;
	int rn = indexed_list[list_index].recN;
	gotoRec (rn);
}

void cbGoLastRec(Fl_Button *b, void *d)
{
	if (!indexed_list) return;
	list_index = netdb.numrecs() - 1;
	gotoRec (indexed_list[list_index].recN);
}

void cb_btnNewSave(Fl_Button *b, void *d)
{
	std::string newPrefix;
	std::string newArea;
	std::string newSuffix;
	std::string chkcall;

	newPrefix.assign(trim(inpPrefix->value()));
	newArea.assign(trim(inpArea->value()));
	newSuffix.assign(trim(inpSuffix->value()));
	chkcall.assign(newPrefix).append(newArea).append(newSuffix);

// do not allow empty database records !!
	if (newPrefix.empty() || newArea.empty() || newSuffix.empty()) {
		fl_alert2("Prefix/Area/Suffix are required entries");
		return;
	}

	if (editState == NEW || editState == ADD) {

		if (netdb.find_callsign(chkcall) == -1) {

			appendNewRecord ();
			getindexed_list();
			SortByPAS();
			currec = binary_search_PAS(0, netdb.numrecs() - 1, newPrefix, newArea, newSuffix);
			gotoRec (currec);

			if (editState == ADD) {
				inp_focus->UpdateWhoIsUp (currec);
			}
			toggleState ();
			update_select_label();
			updateCallins ();
			return;
		} else {
			clearEditForm();
			gotoRec(currec);
			fl_alert ("Callsign already in database");
		}
	} else {
		clearEditForm ();
		editState = NEW;
		showState ();
		inpPrefix->take_focus ();
		return;
	}
	toggleState ();
}

void cb_LookupCall (Fl_Menu_ *m, void *d)
{
	std::string inpCall;
	inpCall.assign(trim(uppercase(inpPrefix->value())));
	inpCall.append(trim(uppercase(inpArea->value())));
	inpCall.append(trim(uppercase(inpSuffix->value())));

	CALLSIGNquery(inpCall);

	return;
}


