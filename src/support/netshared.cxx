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

string sSimpleName;
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

string uppercase( string s)
{
	static string ucase;
	ucase.clear();
	ucase.assign(s);
	for (size_t i = 0; i < s.length(); i++)
		ucase[i] = toupper(ucase[i]);
	return ucase;
}

string trim(string s)
{
	static string trimmed;
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
			btnDelete->show ();
			btnFirst->show ();
			btnPrev->show ();
			btnNext->show ();
			btnLast->show ();
			btn2Queue->show ();
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
			break;
	}
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

long IsInDB (const char *p, const char *a, const char *s)
{
	std::string prefix = trim (uppercase (p));
	std::string area = trim (uppercase(a));
	std::string suffix = trim (uppercase(s));
	long found;
	int cmp, suffix_only = 0;

	found = -1L;
	if (area.empty() && prefix.empty()) suffix_only = 1;

	SortBySAP ();

	for (size_t n = 0; n < netdb.numrecs(); n++) {
		cmp = suffix.compare( trim (indexed_list[n].suffix) );
		if (cmp > 0) continue;
		if (cmp < 0) break;
		// only looking for a suffix match
		if (suffix_only) {
			found = n;
			break;
		}
		if (prefix.compare (trim (indexed_list[n].prefix) ) == 0 &&
			area.compare (trim (indexed_list[n].area) ) == 0) {
			found = n;
			break;
		}
	}
	if (found >= 0) return indexed_list[found].recN;
	return found;
}

// execute after database record modification of any kind
void refresh_logins()
{
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
}

extern Fl_Input *inpCallsign;

void AddNewRecord (const char *prefix, const char *area, const char *suffix)
{
	clearEditForm ();
	inpPrefix->value (prefix);
	inpArea->value (area);
	inpSuffix->value (suffix);
	std::string tmp;
	tmp.assign(trim(prefix)).append(trim(area)).append(trim(suffix));
	inpCallsign->value (tmp.c_str());
	inpNickname->take_focus ();
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
		memset(indexed_list[n].suffix, 0, 4);
		strncpy (indexed_list[n].netnbr, rec.netnbr.c_str(), 4);
		strncpy (indexed_list[n].prefix, rec.prefix.c_str(), 3);
		strncpy (indexed_list[n].area, rec.area.c_str(), 1);
		strncpy (indexed_list[n].suffix, rec.suffix.c_str(), 3);


		while (strlen(indexed_list[n].suffix) < 3)
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
	if (n1 == n2) return 0;
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

void SortBySAP()
{
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), SAPcompare);
}

void SortByNetNbr()
{
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), NetNbrCompare);
}

void SortByAPS()
{
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), APScompare);
}

void SortByPAS()
{
	if (!indexed_list || !netdb.numrecs()) return;
	qsort ( &(indexed_list[0]), netdb.numrecs(), sizeof(index_struct), PAScompare);
}

void SortByPreferred()
{
	switch (progStatus.preferred_sort_order) {
		case 0 : SortByPAS (); break;
		case 1 : SortByAPS (); break;
		case 2 : SortBySAP (); break;
		case 3 : SortByNetNbr (); break;
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
	char dispname[30];
	snprintf(dispname, sizeof(dispname), "%s (%lu)", sSimpleName.c_str(), (unsigned long)netdb.numrecs());
	dbSelectLabel->value(dispname);
	dbSelectLabel->redraw();
}

void openDB(string fname)
{
	sSimpleName = fl_filename_name(fname.c_str());
	netdb.filename(fname.c_str());
	if (netdb.load() != 0) {
		fl_message("Not an flnet csv file");
		exit(0);
	}
	if (netdb.numrecs()) {
		callinlist.setPri_1 (progStatus.chP1[0]);
		callinlist.setPri_2 (progStatus.chP2[0]);
		callinlist.setPri_3 (progStatus.chP3[0]);
		if (progStatus.chAuto == 'y') callinlist.AutoPriority (1);
		getindexed_list();
	} else {
		update_select_label();
	}
}

void dispRec ()
{
	static stringstream fname;
	fname.seekp(ios::beg);
	fname << "File:" << sSimpleName;
	lblFileName->value (fname.str().c_str());
	static stringstream recs;
	recs.seekp(ios::beg);
	recs << "Recs: " << netdb.numrecs();
	lblNumRecs->value (recs.str().c_str());

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


int add_fldigi_record(void)
{
	std::string prefix, area, suffix;

	if (!fldigi_online) {
		fl_alert ("FLDIGI<->FLNET XMLRPC Commmunication failure!");
		return -1;
	}

	struct callsign_data *data = update_flnet_calldata();

	if(!data) {
		fl_alert ("XMLRPC Internal Data transfer failure!");
		return -1;
	}

	if(split_call(data->callsign, prefix, area, suffix) == false) return -1;

	if(IsInDB (prefix.c_str(), area.c_str(), suffix.c_str()) < 0) {
		csvRecord *rec = new csvRecord;
		char date[32];
		time_t rawtime = 0;
		struct tm * timeinfo = 0;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		memset(date, 0, sizeof(date));
		snprintf(date, sizeof(date)-1, "%04d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

		rec->prefix    = trim(uppercase(prefix.c_str()));
		rec->area      = trim(area.c_str());
		rec->suffix    = trim(uppercase(suffix.c_str()));
		rec->name      = trim(data->name.c_str());
		rec->fname     = trim(data->name.c_str());
		rec->city      = trim(data->qth.c_str());
		rec->state     = trim(data->state.c_str());
		rec->callsign  = trim(uppercase(data->callsign.c_str()));
		rec->netnbr    = "";
		rec->logdate   = date;
		rec->lname     = "";
		rec->addr      = "";
		rec->zip       = "";
		rec->phone     = "";
		rec->birthdate = "";
		rec->spouse    = "";
		rec->sp_birth  = "";
		rec->nbrlogins = "";
		rec->status    = "";
		rec->joined    = date;
		rec->comment1  = "";
		rec->comment2  = "";
		rec->email     = "";
		rec->prevdate  = "";
		rec->country   = "";
		rec->locator   = "";

		netdb.add(*rec);
		getindexed_list ();
	}

	delete data;

	return (int) IsInDB (prefix.c_str(), area.c_str(), suffix.c_str());
}

bool split_call(std::string src, std::string &pre, std::string &area, std::string &post)
{
	if(src.empty()) return false;

	int index = 0;
	int start = 0;
	int end = 0;
	std::string temp;

	for(index = src.size() - 1; index > -1; index--) {
		if(src[index] <= ' ') continue;
		if(isdigit(src[index])) break;
		if(!end) end = index;
		start = index;
	}

	temp.clear();
	for(int j = start; j <= end; j++)
		temp += toupper(src[j]);
	post.assign(temp);

	start = 0;
	end = 0;

	for(; index > -1; index--) {
		if(src[index] <= ' ') continue;
		if(isalpha(src[index])) break;
		if(!end) end = index;
		start = index;
	}

	temp.clear();
	for(int j = start; j <= end; j++)
		temp += src[j];
	area.assign(temp);

	start = 0;
	end = 0;

	for(; index > -1; index--) {
		if(src[index] <= ' ') continue;
		if(!isalnum(src[index])) break;
		if(!end) end = index;
		start = index;
	}

	temp.clear();
	for(int j = start; j <= end; j++)
		temp += toupper(src[j]);
	pre.assign(temp);

	return true;
}

void cb_mnuFldigiEditor(Fl_Menu_*, void*)
{
	int rn = add_fldigi_record();
	if(rn < 0) return;
	gotoRec(indexed_list[rn].recN);
}

void cb_F12(int WhoIsUp)
{
	Fl_Window *editor = getEditWindow();
//	editor->resize(main_window->x() + main_window->w() + 10, main_window->y(), 535, 460);

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
		csvRecord *rec = new csvRecord;
		char date[32];
		time_t rawtime = 0;
		struct tm * timeinfo = 0;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		memset(date, 0, sizeof(date));
		snprintf(date, sizeof(date)-1, "%04d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

		rec->prefix.assign(prefix);
		rec->area.assign(area);
		rec->suffix.assign(suffix);
		rec->name.clear();
		rec->fname.clear();
		rec->city.clear();
		rec->state.clear();
		rec->callsign.assign(trim(prefix)).append(trim(area)).append(trim(suffix));
		rec->netnbr.clear();
		rec->logdate.clear();
		rec->lname.clear();
		rec->addr.clear();
		rec->zip.clear();
		rec->phone.clear();
		rec->birthdate.clear();
		rec->spouse.clear();
		rec->sp_birth.clear();
		rec->nbrlogins.clear();
		rec->status.clear();
		rec->joined.clear();
		rec->comment1.clear();
		rec->comment2.clear();
		rec->email.clear();
		rec->prevdate.clear();
		rec->country.clear();
		rec->locator.clear();

		netdb.add(*rec);
		getindexed_list ();

		SortBySAP();
		rnbr = binary_search_SAP(0, netdb.numrecs(), prefix, area, suffix);
	}
	SortByPreferred ();
	editor->show();
	show_sort_order ();
	ModifyRecord(rnbr);
}

void cb_ShiftF12(void)
{
	int rn = add_fldigi_record();
	if(rn < 0) return;
	myUI->PickedToCallinsDB((size_t) indexed_list[rn].recN);
}

void cbEditor ()
{
	Fl_Window *editor = getEditWindow();

	SortByPreferred ();
	editor->show ();
	show_sort_order();
	clearEditForm ();
	editState = UPDATE;
	showState ();
	cbGoFirstRec (NULL, NULL);
}

void cbCloseEditor ()
{
	Fl_Window *editor = getEditWindow();
	editState = UPDATE;
	showState ();
	editor->hide ();
	getindexed_list ();
	refresh_logins ();
	updateCallins ();
}
void cb_btnCancelCallsignSearch(Fl_Button*, void*)
{
	CallsignBrowse->hide();
}

void cb_OkCallsignSearch(Fl_Button*, void*)
{
	int selrec = brwsCallsign->value();
	CallsignBrowse->hide();
	if (selrec != 0) {
		list_index = selrec - 1;
		gotoRec ( indexed_list[list_index].recN);
	}
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
	CallsignBrowse->show();
}

void cb_btnCancelNetNbrSearch(Fl_Button*, void*)
{
	NetNbrBrowse->hide();
}

void cb_OkNetNbrSearch(Fl_Button*, void*)
{
	int selrec = brwsNetNbr->value();
	NetNbrBrowse->hide();
	if (selrec != 0) {
		list_index = selrec - 1;
		gotoRec (indexed_list[list_index].recN);
	}
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
		if (p2 > p) return binary_search_SAP(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare area
		if (a2 > a) return binary_search_SAP(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare suffix
		if (s2 > s) return binary_search_SAP(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_SAP(mid + 1, r, p, a, s);
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
		if (a2 > a) return binary_search_SAP(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare prefix
		if (p2 > p) return binary_search_SAP(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_SAP(mid + 1, r, p, a, s);
		// compare suffix
		if (s2 > s) return binary_search_SAP(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_SAP(mid + 1, r, p, a, s);
		list_index = mid;
		return indexed_list[mid].recN;
	} 
	return -1; 
} 

void cb_btnSearchOK(Fl_Return_Button *b, void *d)
{
	long found;
	if (!indexed_list) return;

	CallsignSearch->hide ();

	SortBySAP();
	out_sorted_by->value("S/A/P");
	out_sorted_by->redraw();

	std::string p = trim(uppercase(sSrchPrefix->value()));
	std::string a = trim(uppercase(sSrchArea->value()));
	std::string s = trim(uppercase(sSrchSuffix->value()));
	found = binary_search_SAP(	0, netdb.numrecs() - 1, p, a, s);

	if (found > -1) {
		gotoRec(found);
	}
}

void cb_mnuSearchCallsign (Fl_Menu_ *m, void *d)
{
	if (!CallsignSearch)
		CallsignSearch = newSearchCallsignDialog();
	sSrchPrefix->value("");
	sSrchArea->value("");
	sSrchSuffix->value("");
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

void cb_btnSearchNetNbrOK (Fl_Return_Button *b, void *d)
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

void cb_mnuSearchNetNbr (Fl_Menu_ *m, void *d)
{
	if (!NetNbrSearch)
		NetNbrSearch = newSearchNetNbrDialog ();
	sSrchNetNbr->value("");
	NetNbrSearch->show ();
}

void cb_btn2Queue(Fl_Button *b, void *d)
{
	myUI->PickedToCallinsDB ((size_t)currec);
}

void cb_btnUpdateCancel(Fl_Button *b, void *d)
{
	Fl_Window *editor = getEditWindow();

	switch (editState) {
		case ADD :
			toggleState ();
			editor->hide ();
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
			editor->hide ();
			refresh_logins ();
			updateCallins ();
			break;
		default : // must be an UPDATE
			if (netdb.numrecs() > 0) {
				std::string p, a, s;
				p = inpPrefix->value();
				a = inpArea->value();
				s = inpSuffix->value();
				saveCurRecord ();
				currec = binary_search_SAP(0, netdb.numrecs(), p, a, s);
				gotoRec (currec);
				update_select_label();
			}
			getindexed_list ();
			refresh_logins ();
	}
}

void cb_btnDelete(Fl_Button *b, void *d)
{
	if (fl_choice("Confirm Delete", "cancel", "OK", NULL) == 1) {
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
	gotoRec (indexed_list[list_index].recN);
}

void cbGoNextRec(Fl_Button *b, void *d)
{
	if (!indexed_list || list_index == (netdb.numrecs() - 1)) return;
	list_index++;
	if (list_index >= netdb.numrecs()) list_index = netdb.numrecs() - 1;
	gotoRec (indexed_list[list_index].recN);
}

void cbGoLastRec(Fl_Button *b, void *d)
{
	if (!indexed_list) return;
	list_index = netdb.numrecs() - 1;
	gotoRec (indexed_list[list_index].recN);
}

void cb_btnNewSave(Fl_Button *b, void *d)
{
	if (editState == NEW || editState == ADD) {
		if (IsInDB (inpPrefix->value(), inpArea->value(), inpSuffix->value()) == -1) {

			std::string newPrefix;
			std::string newArea;
			std::string newSuffix;

			newPrefix.assign(trim(inpPrefix->value()));
			newArea.assign(trim(inpArea->value()));
			newSuffix.assign(trim(inpSuffix->value()));
// do not allow empty database records !!
			if (newPrefix.empty() || newArea.empty() || newSuffix.empty()) {
				fl_alert2("Prefix/Area/Suffix are required entries");
				return;
			}

			appendNewRecord ();
			SortBySAP();

			if(newPrefix.size() && newArea.size() && newSuffix.size()) {
				long found =  IsInDB (newPrefix.c_str(), newArea.c_str(), newSuffix.c_str());

				if (found > -1) {
					currec = found;
					gotoRec(currec);
					toggleState ();
					return;
				}
			}

			if (editState == ADD) {
				Fl_Window *editor = getEditWindow();
				myUI->UpdateWhoIsUp (netdb.numrecs() - 1);
				toggleState ();
				editor->hide ();
				updateCallins ();
				return;
			}
			update_select_label();
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
	string inpCall;
	inpCall.assign(trim(uppercase(inpPrefix->value())));
	inpCall.append(trim(uppercase(inpArea->value())));
	inpCall.append(trim(uppercase(inpSuffix->value())));

	CALLSIGNquery(inpCall);

	return;
}


