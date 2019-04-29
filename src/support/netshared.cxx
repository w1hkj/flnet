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
#include "netsupport.h"
#include "config.h"
#include "net_config.h"
#include "xml_io.h"

#include "csvdb.h"

csvdb netdb;

extern loglist callinlist;

short	rc;
int		fc, nrecs;

brwsStruct *brwsData = NULL;
static int brwstabs[] = { 80, 0 };

State editState = UPDATE;

size_t		currec;
size_t		brwsnum;
size_t		AddedRecNbr;

char sSimpleName[120];
char szDispName[80];

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

char *uppercase (const char *s)
{
	static char ucase[128];
	memset (ucase, (char)0, sizeof(ucase));
	strncpy (ucase, s, sizeof(ucase)-1);
	for (unsigned int i = 0; i < strlen(ucase); i++)
		ucase[i] = toupper(ucase[i]);
	return ucase;
}

char *trim (const char *s)
{
	static char trimmed[128];
	memset (trimmed, (char)0, sizeof(trimmed));
	strncpy (trimmed, s, sizeof(trimmed)-1);
	while ( (strlen(trimmed)) && (trimmed[strlen(trimmed)-1] == ' '))
		trimmed[strlen(trimmed)-1] = 0;
	if (trimmed[0] == 0) return trimmed;
	while (trimmed[0] == ' ')
		strcpy(trimmed, trimmed+1);
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
			btnDelete->show ();
			btnFirst->show ();
			btnPrev->show ();
			btnNext->show ();
			btnLast->show ();
			btn2Queue->show ();
			break;

		case NEW:
			btnNewSave->label ("Save");
			btnUpdateCancel->label ("Cancel");
			btnNewSave->show ();
			btnDelete->hide ();
			btnDelete->show ();
			btnFirst->show ();
			btnPrev->show ();
			btnNext->show ();
			btnLast->show ();
			btn2Queue->show ();
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
	char prefix[3], area[2], suffix[4];
	long found;
	int cmp, suffix_only = 0;
	memset (prefix, 0, 3);
	memset (area, 0, 2);
	memset (suffix, 0, 4);
	strncpy (prefix, trim (uppercase (p)), 2);
	strncpy (area, trim (uppercase (a)), 1);
	strncpy (suffix, trim (uppercase (s)), 3);

	found = -1L;
	if (area[0] == 0 && prefix[0]== 0) suffix_only = 1;

	SortBySAP ();

	for (int n = 0; n < netdb.numrecs(); n++) {
		cmp = strcmp (suffix, trim (brwsData[n].suffix));
		if (cmp > 0) continue;
		if (cmp < 0) break;
		// only looking for a suffix match
		if (suffix_only) {
			found = n;
			break;
		}
		if (strcmp(prefix, trim (brwsData[n].prefix)) == 0 &&
			strcmp(area, brwsData[n].area) == 0) {
			found = n;
			break;
		}
	}
	return found;
}

extern Fl_Input *inpCallsign;

void AddNewRecord (char *prefix, char *area, char *suffix)
{
	char tmp[12];
	clearEditForm ();
	inpPrefix->value (prefix);
	inpArea->value (area);
	inpSuffix->value (suffix);
	strcpy (tmp, trim(prefix));
	strcat (tmp, trim(area));
	strcat (tmp, trim(suffix));
	inpCallsign->value (tmp);
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


void getBrwsData()
{
	if (!netdb.numrecs()) {
		return;
	}

	snprintf (szDispName, sizeof(szDispName), "%s (%d)", sSimpleName, (int)netdb.numrecs());
	dbSelectLabel->value(szDispName);
	dbSelectLabel->redraw();

	currec = netdb.recnbr();
	if (brwsData) {
		delete[] brwsData;
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

void openDB(string fname)
{
	strcpy (sSimpleName, fl_filename_name(fname.c_str()));
	netdb.filename(fname.c_str());
	if (netdb.load() != 0) {
		fl_message("Not an flnet csv file");
		exit(0);
	}
	if (netdb.numrecs()) {
		readConfig ();
		callinlist.setPri_1 (chP1[0]);
		callinlist.setPri_2 (chP2[0]);
		callinlist.setPri_3 (chP3[0]);
		if (chAuto == 'y') callinlist.AutoPriority (1);
		getBrwsData();
	} else {
		snprintf (szDispName, sizeof(szDispName), "%s (%d)", sSimpleName, (int)netdb.numrecs());
//		dbSelectGroup->label (szDispName);
//		dbSelectGroup->redraw();
		dbSelectLabel->value(szDispName);
		dbSelectLabel->redraw();
	}
}

void dispRec ()
{
	char buf[80];
	sprintf (buf,"File:%s",sSimpleName);
	lblFileName->value (buf);
	sprintf (buf,"Recs: %d", (int)netdb.numrecs());
	lblNumRecs->value (buf);

	csvRecord rec;
	netdb.get(currec, rec);

	inpPrefix->value (trim (rec.prefix.c_str()));
	inpArea->value (trim (rec.area.c_str()));
	inpSuffix->value (trim (rec.suffix.c_str()));
	inpNickname->value (trim (rec.name.c_str()));
	inpNetNbr->value (trim (rec.netnbr.c_str()));
	txtLogDate->value( rec.logdate.c_str());
	inpFname->value (trim (rec.fname.c_str()));
	inpLname->value (trim (rec.lname.c_str()));
	inpAddress->value (trim (rec.addr.c_str()));
	inpCity->value (trim (rec.city.c_str()));
	inpState->value (trim (rec.state.c_str()));
	inpZip->value (trim (rec.zip.c_str()));
	strcpy(buf, rec.phone.c_str());
	if (strpbrk (buf, "0123456789") != NULL)
		inpPhone->value (trim (buf));
	else
		inpPhone->value ("");
	inpSpouse->value (trim (rec.spouse.c_str()));
	inpSpBirthday->value (trim (rec.sp_birth.c_str()));
	inpBirthday->value(rec.birthdate.c_str());

	inpCallsign->value (trim (rec.callsign.c_str()));
	inpNbrLogins->value (trim (rec.nbrlogins.c_str()));
	inpStatus->value (trim (rec.status.c_str()));
	inpJoined->value (trim (rec.joined.c_str()));
	inpComment1->value (trim (rec.comment1.c_str()));
	inpComment2->value (trim (rec.comment2.c_str()));
	inpEmail->value (trim (rec.email.c_str()));
	inpPrevDate->value (trim (rec.prevdate.c_str()));

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
	getBrwsData ();
}

void appendNewRecord (csvRecord &rec)
{
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
		csvRecord rec;
		char date[32];
		time_t rawtime = 0;
		struct tm * timeinfo = 0;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		memset(date, 0, sizeof(date));
		snprintf(date, sizeof(date)-1, "%04d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday);

		rec.prefix    = trim(uppercase(prefix.c_str()));
		rec.area      = trim(area.c_str());
		rec.suffix    = trim(uppercase(suffix.c_str()));
		rec.name      = trim(data->name.c_str());
		rec.fname     = trim(data->name.c_str());
		rec.city      = trim(data->qth.c_str());
		rec.state     = trim(data->state.c_str());
		rec.callsign  = trim(uppercase(data->callsign.c_str()));
		rec.netnbr 	  = "";
		rec.logdate   = date;
		rec.lname 	  = "";
		rec.addr	  = "";
		rec.zip	      = "";
		rec.phone     = "";
		rec.birthdate = "";
		rec.spouse    = "";
		rec.sp_birth  = "";
		rec.nbrlogins = "";
		rec.status    = "";
		rec.joined    = date;
		rec.comment1  = "";
		rec.comment2  = "";
		rec.email     = "";
		rec.prevdate  = "";

		netdb.add(rec);
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
	long shownrec;
	int n;
	n = callinlist.numlist ();
	if (!n) return;

	Fl_Window *editor = getEditWindow();
	editor->resize(main_window->x() + main_window->w() + 10, main_window->y(), 535, 460);

	SortBySAP ();
	clearEditForm ();
	editor->show ();

	if ((shownrec = callinlist.recN (WhoIsUp)) != -1)
		ModifyRecord (shownrec);
	else
		AddNewRecord (callinlist.prefix (WhoIsUp),
					  callinlist.area (WhoIsUp),
					  callinlist.suffix (WhoIsUp));
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
	editor->resize(main_window->x() + main_window->w() + 10, main_window->y(), 535, 460);

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
	getBrwsData ();
	SortBySAP ();
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

void cb_btnSearchOK(Fl_Return_Button *b, void *d)
{
	long found;
	if (!brwsData) return;

	CallsignSearch->hide ();

	found = IsInDB (sSrchPrefix->value(), sSrchArea->value(), sSrchSuffix->value());
	if (found > -1) {
		brwsnum = found;
		gotoRec (brwsData[found].recN);
	}
}

void cb_mnuSearchCallsign (Fl_Menu_ *m, void *d)
{
	if (!CallsignSearch)
		CallsignSearch = newSearchCallsignDialog();
	CallsignSearch->show();
}

void cb_btnSearchNetNbrCancel (Fl_Button *b, void *d)
{
	NetNbrSearch->hide ();
}

void cb_btnSearchNetNbrOK (Fl_Return_Button *b, void *d)
{
	if (!brwsData) return;
	long found;
	char szNbr[10];
	int nbr, netnbr;

	strcpy (szNbr, sSrchNetNbr->value());
	NetNbrSearch->hide ();

	found = -1L;
	SortByNetNbr ();
	nbr = atoi(szNbr);
	for (int n = 0; n < netdb.numrecs(); n++) {
		netnbr = atoi (brwsData[n].netnbr);
		if (nbr > netnbr) continue;
		if (nbr < netnbr) break;
		found = n;
		break;
	}
	if (found > -1) {
		brwsnum = found;
		gotoRec (brwsData[found].recN);
	}
}

void cb_mnuSearchNetNbr (Fl_Menu_ *m, void *d)
{
	if (!NetNbrSearch)
		NetNbrSearch = newSearchNetNbrDialog ();
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
		}
	}
}

void cb_btnDelete(Fl_Button *b, void *d)
{
	if (fl_choice("Confirm Delete", "cancel", "OK", NULL) == 1) {
		netdb.erase(currec);
		cbGoFirstRec (NULL,NULL);
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

			appendNewRecord ();

			getBrwsData ();
			SortBySAP();

			if(newPrefix.size() && newArea.size() && newSuffix.size()) {
				long found =  IsInDB (newPrefix.c_str(), newArea.c_str(), newSuffix.c_str());
				if(found > -1) {
					currec = brwsData[found].recN;
					gotoRec(currec);
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
		} else
			fl_alert ("Callsign already in database");
		//cbGoLastRec (NULL, NULL);
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
	string tcall = trim(uppercase(inpPrefix->value()));
	tcall.append(trim(uppercase(inpArea->value())));
	tcall.append(trim(uppercase(inpSuffix->value())));

std::cout << tcall << std::endl;

//	inpFname->value (buff);
//	inpLname->value (buff);
//	inpAddress->value (buff);
//	inpCity->value (buff);
//	inpState->value (buff);
//	inpZip->value (buff);

	return;
}

