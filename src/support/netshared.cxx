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

int  binary_search_call(int l, int r, std::string &p, std::string &a, std::string &s);
int  binary_search_netnbr(int l, int r, int x);

short	rc;
int		fc, nrecs;

brwsStruct *brwsData = NULL;
static int brwstabs[] = { 80, 0 };

State editState = UPDATE;

size_t		currec;
size_t		brwsnum;
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

	for (int n = 0; n < netdb.numrecs(); n++) {
		cmp = suffix.compare( trim (brwsData[n].suffix) );
		if (cmp > 0) continue;
		if (cmp < 0) break;
		// only looking for a suffix match
		if (suffix_only) {
			found = n;
			break;
		}
		if (prefix.compare (trim (brwsData[n].prefix) ) == 0 &&
			area.compare (trim (brwsData[n].area) ) == 0) {
			found = n;
			break;
		}
	}
	if (found >= 0) return brwsData[found].recN;
	return found;
}

extern Fl_Input *inpCallsign;

void AddNewRecord (const char *prefix, const char *area, const char *suffix)
{
//std::cout << "AddNewRecord(" << prefix << area << suffix << ")" << std::endl;
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
void getBrwsData()
{
//std::cout << "genBrwsData() " << ++get_cnt << std::endl;
	if (!netdb.numrecs()) {
		return;
	}

	update_select_label();

	currec = netdb.recnbr();
	if (brwsData) {
//std::cout << "numrecs " << netdb.numrecs() << std::endl;
//std::cout << "brwsData " << brwsData << std::endl;
//std::cout << "sizeof(brwsdata) " << sizeof(brwsData) << std::endl;
		delete [] brwsData;
		brwsData = NULL;
	}

	csvRecord rec;
	brwsData = new brwsStruct[netdb.numrecs()];
	for (int n = 0; n < netdb.numrecs(); n++) {
		netdb.get(n, rec);
		brwsData[n].recN = n;
		memset(brwsData[n].netnbr, 0, 5);
		memset(brwsData[n].prefix, 0, 4);
		memset(brwsData[n].area, 0, 2);
		memset(brwsData[n].suffix, 0, 4);
		strncpy (brwsData[n].netnbr, rec.netnbr.c_str(), 4);
		strncpy (brwsData[n].prefix, rec.prefix.c_str(), 3);
		strncpy (brwsData[n].area, rec.area.c_str(), 1);
		strncpy (brwsData[n].suffix, rec.suffix.c_str(), 3);
		while (strlen(brwsData[n].suffix) < 3)
			strcat (brwsData[n].suffix, " ");
	}
	netdb.get(currec, rec);
}

int NetNbrCompare (const void *p1, const void *p2)
{
	brwsStruct *s1 = (brwsStruct *)p1;
	brwsStruct *s2 = (brwsStruct *)p2;
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
	brwsStruct *s1 = (brwsStruct *)p1;
	brwsStruct *s2 = (brwsStruct *)p2;
	int cmp;
	if ((cmp = strcmp(s1->suffix, s2->suffix)) != 0) return cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	return strcmp(s1->prefix, s2->prefix);
}

int APScompare (const void *p1, const void *p2)
{
	brwsStruct *s1 = (brwsStruct *)p1;
	brwsStruct *s2 = (brwsStruct *)p2;
	int cmp;
	if ((cmp = strcmp(s1->area, s2->area)) != 0) return cmp;
	if ((cmp = strcmp(s1->prefix, s2->prefix)) != 0) return cmp;
	return strcmp(s1->suffix, s2->suffix);
}

void SortBySAP()
{
	if (!brwsData || !netdb.numrecs()) return;
	qsort ( &(brwsData[0]), netdb.numrecs(), sizeof(brwsStruct), SAPcompare);
}

void SortByNetNbr()
{
	if (!brwsData || !netdb.numrecs()) return;
	qsort ( &(brwsData[0]), netdb.numrecs(), sizeof(brwsStruct), NetNbrCompare);
}

void SortByAPS()
{
	if (!brwsData || !netdb.numrecs()) return;
	qsort ( &(brwsData[0]), netdb.numrecs(), sizeof(brwsStruct), APScompare);
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
	snprintf(dispname, sizeof(dispname), "%s (%d)", sSimpleName.c_str(), netdb.numrecs());
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
		getBrwsData();
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
//std::cout << "appendNewRecord()" << std::endl;
	csvRecord rec;
	setFields (rec);
	netdb.add(rec);
	getBrwsData ();
}

void appendNewRecord (csvRecord &rec)
{
//std::cout << "appendNewRecord(rec)" << std::endl;
	setFields (rec);
	netdb.add(rec);
	getBrwsData ();
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
		getBrwsData ();
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
	gotoRec(brwsData[rn].recN);
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

	int rnbr = binary_search_call(0, netdb.numrecs(), prefix, area, suffix);

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
		getBrwsData ();

		SortBySAP();
		rnbr = binary_search_call(0, netdb.numrecs(), prefix, area, suffix);
	}
	editor->show();
	ModifyRecord(brwsData[rnbr].recN);
}

void cb_ShiftF12(void)
{
	int rn = add_fldigi_record();
	if(rn < 0) return;
	myUI->PickedToCallinsDB((size_t) brwsData[rn].recN);
}

void cbEditor ()
{
	Fl_Window *editor = getEditWindow();
//	editor->resize(main_window->x() + main_window->w() + 10, main_window->y(), 535, 460);

	SortBySAP ();
	clearEditForm ();
	editor->show ();
	editState = UPDATE;
	showState ();
	cbGoFirstRec (NULL, NULL);
}

void cbCloseEditor ()
{
	Fl_Window *editor = getEditWindow();
//	getBrwsData ();
//	SortBySAP ();
	editState = UPDATE;
	showState ();
	editor->hide ();
	myUI->dispCallIns (false);
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
		brwsnum = selrec - 1;
		gotoRec ( brwsData[brwsnum].recN);
	}
}

void cb_mnuBrowseCallsign (Fl_Menu_*, void*)
{
	char brwsLine[30];
	if (!CallsignBrowse) CallsignBrowse = winCallsignBrowse();
	getBrwsData();
	SortByAPS();
	brwsCallsign->column_widths(brwstabs);
	brwsCallsign->clear ();
	for (int i = 0; i < netdb.numrecs(); i++) {
		strcpy(brwsLine, brwsData[i].netnbr);
		strcat(brwsLine, "\t");
		strcat(brwsLine, brwsData[i].prefix);
		strcat(brwsLine, brwsData[i].area);
		strcat(brwsLine, brwsData[i].suffix);
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
		brwsnum = selrec - 1;
		gotoRec (brwsData[brwsnum].recN);
	}
}

void cb_mnuBrowseNetNbr (Fl_Menu_*, void*)
{
	char brwsLine[30];
	if (!NetNbrBrowse) NetNbrBrowse = winNetNbrBrowse();
	getBrwsData();
	SortByNetNbr();
	brwsNetNbr->clear ();
	brwsNetNbr->column_widths(brwstabs);
	for (int i = 0; i < netdb.numrecs(); i++) {
		strcpy(brwsLine, brwsData[i].netnbr);
		strcat(brwsLine, "\t");
		strcat(brwsLine, brwsData[i].prefix);
		strcat(brwsLine, brwsData[i].area);
		strcat(brwsLine, brwsData[i].suffix);
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
int  binary_search_call(int l, int r, std::string &p, std::string &a, std::string &s) 
{ 
	std::string p2, a2, s2;
	if (r >= l) { 
		int mid = l + (r - l) / 2; 
		p2 = trim(brwsData[mid].prefix);
		a2 = trim(brwsData[mid].area);
		s2 = trim(brwsData[mid].suffix);
		if (s2 > s) return binary_search_call(l, mid - 1, p, a, s);
		if (s2 < s) return binary_search_call(mid + 1, r, p, a, s);
		if (a2 > a) return binary_search_call(l, mid - 1, p, a, s);
		if (a2 < a) return binary_search_call(mid + 1, r, p, a, s);
		if (p2 > p) return binary_search_call(l, mid - 1, p, a, s);
		if (p2 < p) return binary_search_call(mid + 1, r, p, a, s);
		return mid;
	} 
	return -1; 
} 

void cb_btnSearchOK(Fl_Return_Button *b, void *d)
{
	long found;
	if (!brwsData) return;

	CallsignSearch->hide ();

	SortBySAP();
	std::string p = trim(uppercase(sSrchPrefix->value()));
	std::string a = trim(uppercase(sSrchArea->value()));
	std::string s = trim(uppercase(sSrchSuffix->value()));
	found = binary_search_call(	0, netdb.numrecs() - 1, p, a, s);

	if (found > -1) {
		gotoRec(brwsData[found].recN);
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
int  binary_search_netnbr(int l, int r, int x) 
{ 
	int snbr;
    if (r >= l) { 
        int mid = l + (r - l) / 2; 
        snbr = atol(brwsData[mid].netnbr);
        if (snbr == x) return mid; 
        if (snbr > x) return binary_search_netnbr(l, mid - 1, x); 
        return binary_search_netnbr(mid + 1, r, x); 
    } 
    return -1; 
} 

void cb_btnSearchNetNbrOK (Fl_Return_Button *b, void *d)
{
	if (!brwsData) return;
	int srchnbr = atol(sSrchNetNbr->value());
	int found = -1;
	NetNbrSearch->hide ();

	SortByNetNbr ();
	found = binary_search_netnbr(0, netdb.numrecs() - 1, srchnbr);

	if (found == -1) return;

	gotoRec(brwsData[found].recN);
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

	if (editState == ADD) {
		toggleState ();
		editor->hide ();
		return;
	}
	if (editState == NEW) {
		clearEditForm ();
		dispRec ();
		toggleState ();
		return;
	}
	if (editState == MODIFY) {
		callinlist.modify (WhoIsUp, currec,
						   inpPrefix->value(),
						   inpArea->value (),
						   inpSuffix->value (),
						   inpNickname->value ());
		saveCurRecord ();
		getBrwsData ();
		SortBySAP ();
		toggleState ();
		editor->hide ();
		updateCallins (false);
	} else { // must be an UPDATE in normal editor mode
		if (netdb.numrecs() > 0) {
			saveCurRecord ();
			getBrwsData ();
			SortBySAP ();
			update_select_label();
		}
	}
}

void cb_btnDelete(Fl_Button *b, void *d)
{
	if (fl_choice("Confirm Delete", "cancel", "OK", NULL) == 1) {
		netdb.erase(currec);
		cbGoFirstRec (NULL,NULL);
		update_select_label();
	}
}

void cbGoFirstRec(Fl_Button *b, void *d)
{
	if (!brwsData) return;
	brwsnum = 0;
	gotoRec (brwsData[brwsnum].recN);
}

void cbGoPrevRec(Fl_Button *b, void *d)
{
	if (!brwsData || brwsnum == 0) return;
	brwsnum--;
	gotoRec (brwsData[brwsnum].recN);
}

void cbGoNextRec(Fl_Button *b, void *d)
{
	if (!brwsData || (int)brwsnum == (netdb.numrecs() - 1)) return;
	brwsnum++;
	gotoRec (brwsData[brwsnum].recN);
}

void cbGoLastRec(Fl_Button *b, void *d)
{
	if (!brwsData) return;
	brwsnum = netdb.numrecs() - 1;
	gotoRec (brwsData[brwsnum].recN);
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
				updateCallins (false);
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


