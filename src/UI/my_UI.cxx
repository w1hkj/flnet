// =====================================================================
//
// my_UI.cxx
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_ask.H>

#include "my_UI.h"
#include "loglist.h"
#include "netshared.h"
#include "netsupport.h"
#include "config.h"
#include "net_config.h"
#include "net_ui.h"
#include "timeops.h"
#include "xml_io.h"
#include "debug.h"

#include "csvdb.h"

char emptyline[] = "";

loglist callinlist;
int	WhoIsUp = 0;
static int lastUp;

char keyval[2] = " ";
string szPrefix = "";
string szArea = "";
string szSuffix = "";
string szFullName;
string szLogDate;
string szBirthday;
string szNetNbr;
string szSpouse;
string szPhone;
string szQTH;
string szEmail;
string szComment1;
string szComment2;
string szFirstName;
string szCallSign;

char szInfo[1024];

bool updateFldigi = true;

// do not change this unless you also change txtPick[]
#define NPICKITEMS 10
struct _pick {
	long recN;
	char	callsign[10];
};

static _pick Pick[NPICKITEMS];
static int nbrPicked;
static int whoPicked;

char *fmtDate(char *d)
{
	static char date[11];
	int i;
	strcpy (date, "  /  /    ");
	for (i = 0; i < 2; i++) date[i] = d[i+4];
	for (i = 0; i < 2; i++) date[i+3] = d[i+6];
	for (i = 0; i < 4; i++) date[i+6] = d[i];
	return date;
}

void updateCallins (bool fldigi_flag)
{
	int i,j;

	for (i = WhoIsUp - 4, j = 0; i < WhoIsUp + 11; i++, j++) {
		txtLine[j]->labelcolor (fgColors[callinlist.status(i)]);
		txtLine[j]->color (bgColors[callinlist.status(i)]);
		txtLine[j]->label (callinlist.displine(i));
	}

	szFullName.clear();
	szLogDate.clear();
	szBirthday.clear();
	szNetNbr.clear();
	szSpouse.clear();
	szPhone.clear();
	szEmail.clear();
	szQTH.clear();
	szFirstName.clear();
	szCallSign.clear();

	static char szInList[13];
	sprintf (szInList, "%3d callins", callinlist.numlist ());
	txtNcallins->label (szInList);

	long rc = callinlist.recN(WhoIsUp);
	static string sInfo;
	sInfo.clear();
	if (rc >= 0 && rc < (long)netdb.numrecs()) {
		csvRecord rec;
		netdb.get(rc, rec);

		if(fldigi_flag) {
			if(rec.name.empty())
				szFirstName.assign(trim(rec.fname.c_str()));
			else
				szFirstName.assign(trim(rec.name.c_str()));

			szCallSign.assign(trim(rec.callsign.c_str()));
			update_fldigi_callsign(szFirstName, szCallSign);
		}

		szFullName.assign(trim(rec.fname.c_str())).append(" ");
		szFullName.append(trim(rec.lname.c_str()));
		szLogDate.assign(rec.logdate);
		szNetNbr.assign(rec.netnbr);
		szSpouse.assign(trim(rec.spouse.c_str()));
		if (!szSpouse.empty()) {
			szSpouse.append(", ").append(rec.sp_birth);
		}
		szPhone.assign(trim(rec.phone.c_str()));

		szQTH.assign(trim(rec.city.c_str()));
		szQTH.append(", ").append(trim(rec.state.c_str()));

		szBirthday.assign(trim(rec.birthdate.c_str()));

		szEmail.assign(trim(rec.email.c_str()));
		size_t pat = szEmail.find("@");
		if (pat != string::npos) szEmail.insert(pat, "@");

		szInfo[0] = 0;
		snprintf (szInfo, sizeof(szInfo), "Name:   %s", szFullName.c_str());
		while (strlen(szInfo) < 28) strcat(szInfo, " ");
		sInfo.assign(szInfo);

		snprintf (szInfo, sizeof(szInfo), "Nbr: %s\n", szNetNbr.c_str());
		sInfo.append(szInfo);

		snprintf (szInfo, sizeof(szInfo), "Birth:  %s", szBirthday.c_str());
		while (strlen(szInfo) < 28) strcat(szInfo, " ");
		sInfo.append(szInfo).append("Last: ").append(szLogDate).append("\n");

		snprintf (szInfo, sizeof(szInfo), "Spouse: %s", szSpouse.c_str());
		while (strlen(szInfo) < 28) strcat(szInfo, " ");
		sInfo.append(szInfo).append("Phone:\n");

		snprintf (szInfo, sizeof(szInfo), "QTH:    %s", szQTH.c_str());
		while (strlen(szInfo) < 28) strcat(szInfo, " ");
		sInfo.append(szInfo).append(szPhone).append("\n");

		snprintf (szInfo, sizeof(szInfo), "Email:  %-s", szEmail.c_str());

		sInfo.append(szInfo);

		szComment1.assign(rec.comment1);
		szComment2.assign(rec.comment2);
		sInfo.append("\nInfo:\n");
		sInfo.append(szComment1.c_str()).append("\n");
		sInfo.append(szComment2.c_str());
	}
	txtInfo->label (sInfo.c_str());
}

void updateLogins ()
{
	int i, n;
	long rc;
	FILE *fToday;
	char today[80];
	char szLine[40];
	char sztemp[40];
	csvRecord rec;

	struct tm tim;
	time_t t;
	time(&t);
	gmtime_r(&t, &tim);
	strftime(today, sizeof(today), "%Y%m%d", &tim);

	string outfilename = selected_file;
	size_t p = outfilename.find(".csv");
	if (p != string::npos) outfilename.erase(p);
	p = outfilename.find(".CSV");
	if (p != string::npos) outfilename.erase(p);
	outfilename.append("-").append(today).append(".log");

	if (callinlist.numlist() == 0) return;
	if (callinlist.numlist()) {
		fToday = fopen (outfilename.c_str(), "a");
		for (i = 0; i < callinlist.numlist(); i++) {
			strcpy (szLine, callinlist.displine(i));
			fprintf (fToday, "%s\n", szLine);
			rc = callinlist.recN (i);
			if (rc >= 0 && rc < (long)netdb.numrecs()) {
				netdb.get(rc, rec);
				rec.prevdate.assign(rec.logdate);
				n = atoi (rec.nbrlogins.c_str());
				n += 1;
				sprintf (sztemp, "%d", n);
				rec.nbrlogins.assign(sztemp);
				rec.logdate.assign(today);
				netdb.put(rc, rec);
			} else {
				netdb.clearrec(rec);
				szLine[6] = 0;
				char *pos = strpbrk(szLine,"0123456789");
				if (pos != NULL) {
					pos++;
					rec.suffix.assign(pos);
					*pos = 0;
					pos--;
					rec.area.assign(pos);
					*pos = 0;
					rec.prefix.assign(szLine);
					rec.logdate.assign(today);
					rec.nbrlogins.assign("1");
					netdb.add(rec);
				}
			}
		}
	}
	fclose (fToday);
	callinlist.clear ();
	updateCallins (false);
}

void my_UI::UpdateWhoIsUp (long L)
{
	string pr, ar, su, nm;
	csvRecord rec;
	netdb.get(L, rec);
	callinlist.modify (WhoIsUp, L,
					   rec.prefix.c_str(),
					   rec.area.c_str(),
					   rec.suffix.c_str(),
					   rec.name.c_str());
	dispCallIns(false);
}

void my_UI::clearPickList ()
{
	for (int i = 0; i < NPICKITEMS; i++ ) {
		Pick[i].callsign[0] = 0;
		txtPick[i]->label ("");//Pick[i].callsign);
	}
	nbrPicked = 0;
}

void my_UI::fillPickList ()
{
	int i;
	int cmp;
	int rc;
	char padded[4];

	if (!brwsData) getBrwsData ();
	SortBySAP ();

	clearPickList ();
	strcpy (padded, szSuffix.c_str());
	while (strlen(padded) < 3) strcat (padded, " ");

	i = 0; rc = 0;
	while (rc < netdb.numrecs() && padded[0] > brwsData[rc].suffix[0]) rc++;

	while (rc < netdb.numrecs()) {
		cmp = strcmp(padded, brwsData[rc].suffix);
		if (cmp < 0) break;
		if (cmp == 0 && i < NPICKITEMS) {
			Pick[i].recN = brwsData[rc].recN;
			strcpy (Pick[i].callsign, brwsData[rc].prefix);
			strcat (Pick[i].callsign, brwsData[rc].area);
			strcat (Pick[i].callsign, brwsData[rc].suffix);
			txtPick[i]->label (Pick[i].callsign);
			if (i == 0)
				txtPick[i]->labelcolor (FL_RED);
			else
				txtPick[i]->labelcolor (FL_BLACK);
			i++;
		}
		rc++;
	}
	nbrPicked = i;
}

void my_UI::PickedColors ()
{
	int i;
	for (i = 0; i < NPICKITEMS; i++) {
		if (i == whoPicked)
			txtPick[i]->labelcolor(FL_RED);
		else
			txtPick[i]->labelcolor(FL_BLACK);
		txtPick[i]->redraw ();
	}
}

void my_UI::dispCallIns (bool flag)
{
	updateCallins (flag);
}

void my_UI::PickedToCallins (int n)
{
	PickedToCallinsDB((size_t) Pick[n].recN);
}

void my_UI::PickedToCallinsDB (size_t record_number)
{
	time_t the_time;
	struct tm *tm_ptr;
	char pr[3], ar[2], su[4],nm[11], st[3], sztime[6];

	if (callinlist.inList(record_number)) {
		fl_beep (FL_BEEP_ERROR);
		clearPickList ();
		clearSAP ();
		return;
	}
	time (&the_time);
	tm_ptr = localtime (&the_time);
	sprintf( sztime, "%02d:%02d", tm_ptr->tm_hour, tm_ptr->tm_min);

	csvRecord rec;
	netdb.get(record_number, rec);

	memset(pr, 0, sizeof(pr));
	memset(ar, 0, sizeof(ar));
	memset(su, 0, sizeof(su));
	memset(nm, 0, sizeof(nm));
	memset(st, 0, sizeof(st));

	strncpy(pr, rec.prefix.c_str(), sizeof(pr)-1);
	strncpy(ar, rec.area.c_str(), sizeof(ar)-1);
	strncpy(su, rec.suffix.c_str(), sizeof(su)-1);
	strncpy(nm, rec.name.c_str(), sizeof(nm)-1);
	strncpy(st, rec.status.c_str(), sizeof(st)-1);

	if (strstr (szP1, st)) {
		callinlist.add (record_number, pr, ar, su, nm, sztime, chP1[0]);
	} else if (strstr (szP2, st)) {
		callinlist.add (record_number, pr, ar, su, nm, sztime, chP2[0]);
	} else if (strstr (szP3, st)) {
		callinlist.add (record_number, pr, ar, su, nm, sztime, chP3[0]);
	} else {
		callinlist.add (record_number, pr, ar, su, nm, sztime);
	}
	dispCallIns (false);
	clearPickList ();
	clearSAP ();
}

my_UI::my_UI (int x, int y, int w, int h, const char *l) :
Fl_Group(x,y,w,h)
{
	callinlist = loglist ();
	my_status = LOGLIST;
	end();
}

void my_UI::clearSAP ()
{
	szPrefix.clear(); szArea.clear(); szSuffix.clear();
	inpLoginSuffix->labelcolor (FL_BLACK);
	inpLoginSuffix->color (FL_WHITE);
	inpLoginPrefix->labelcolor (FL_BLACK);
	inpLoginPrefix->color (FL_WHITE);
	inpLoginArea->labelcolor (FL_BLACK);
	inpLoginArea->color (FL_WHITE);
	inpLoginPrefix->label (szPrefix.c_str());
	inpLoginArea->label (szArea.c_str());
	inpLoginSuffix->label (szSuffix.c_str());
	my_status = LOGLIST;
}

static int keywait = 0;
static double delaysecs = 0.10;

void updnDelay (void *d)
{
	keywait = 0;
}

int my_UI::handle (int e)
{
	int k;

	if (e == FL_KEYDOWN) {
		k = Fl::event_key();
		if (k) {
			if (my_status == LOGLIST) {
				if ((k == FL_Up || k == FL_Down) && keywait)
					return 1;
				keywait = 1;
				Fl::add_timeout (delaysecs, updnDelay);
				if (k == FL_Up) {
					WhoIsUp--;
					if (WhoIsUp < 0)
						WhoIsUp++;
					dispCallIns (false);
					return 1;
				}
				if (k == FL_Down) {
					WhoIsUp++;
					if (WhoIsUp == callinlist.numlist ())
						WhoIsUp--;
					dispCallIns (false);
					return 1;
				}
			}
		}
	}

	if (e == FL_KEYUP) {
		k = Fl::event_key();
		if (k) {
			if (k == FL_Escape && my_status != LOGLIST) {
				clearSAP ();
				clearPickList ();
				return 1;
			}
			if (my_status == LOGLIST) {
				if (k == FL_Home) {
					lastUp = WhoIsUp;
					WhoIsUp = 0;
					dispCallIns (false);
					return 1;
				}
				if (k == FL_End) {
					lastUp = WhoIsUp;
					WhoIsUp = callinlist.numlist () - 1;
					dispCallIns (false);
					return 1;
				}
				if (k == FL_Left) {
					WhoIsUp = callinlist.nextup ();
					dispCallIns (false);
					return 1;
				}
				if (k == 65451) {
					callinlist.lastup(WhoIsUp);
					return 1;
				}
				if (k == FL_Page_Up) {
					WhoIsUp = callinlist.lastup();
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 1) {
					callinlist.status(WhoIsUp, LOGIN);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 2) {
					callinlist.status(WhoIsUp, FIRST);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 3) {
					callinlist.status(WhoIsUp, SECOND);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 4) {
					callinlist.status(WhoIsUp, LOGOUT);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 5) { // Priority 0 station
					WhoIsUp = callinlist.Pri_0 (WhoIsUp);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 6) { // Priority 1 station
					WhoIsUp = callinlist.Pri_1 (WhoIsUp);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 7) { // Priority 2 station
					WhoIsUp = callinlist.Pri_2 (WhoIsUp);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 8) { // Priority 2 station
					WhoIsUp = callinlist.Pri_3 (WhoIsUp);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 9) { // Move this call up in list
					WhoIsUp = callinlist.MoveEarlier (WhoIsUp);
					dispCallIns (false);
					return 1;
				}
				if (k == FL_F + 10) { // Move this call dn in list
					WhoIsUp = callinlist.MoveLater (WhoIsUp);
					dispCallIns (false);
					return 1;
				}

				if (k == FL_F + 12) {
					if((Fl::event_state() & FL_SHIFT) == FL_SHIFT)
						cb_ShiftF12();
					else
						cb_F12 (WhoIsUp);
					return 1;
				}

				if (k == FL_Delete) {
					if (WhoIsUp == 0 && callinlist.status(WhoIsUp) == EMPTY) return 1;
					fl_beep (FL_BEEP_QUESTION);
					if (fl_choice("Confirm Delete", "cancel", "OK", NULL) == 1) {
						callinlist.del(WhoIsUp);
						WhoIsUp--;
						if (WhoIsUp < 0) WhoIsUp = 0;
						dispCallIns (false);
						return 1;
					}
				}
			}

			if (my_status == LOGLIST) {
				if (k == FL_Enter) {
					dispCallIns(updateFldigi);
					return 1;
				}
			}

			if (my_status == PICKLIST) {
				if (k == FL_Enter) {
					PickedToCallins (whoPicked);
					if (callin_is_up) {
						WhoIsUp = callinlist.numlist () - 1;
						dispCallIns(false);
					}
					my_status = LOGLIST;
				}
				if (k == FL_Down) {
					whoPicked++;
					if (whoPicked > nbrPicked -1) whoPicked = 0;
					PickedColors();
				}
				if (k == FL_Up) {
					whoPicked--;
					if (whoPicked < 0) whoPicked = nbrPicked -1;
					PickedColors();
				}
			}

			if (my_status == SUFFIX && (k == FL_Down || k == FL_Up)) {
				if (nbrPicked) {
					inpLoginSuffix->color (FL_DARK_RED);
					inpLoginSuffix->labelcolor (FL_WHITE);
					inpLoginSuffix->redraw ();
					if (nbrPicked > 1)
						if (k == FL_Down)
							whoPicked = 1;
						else
							whoPicked = nbrPicked - 1;
						else
							whoPicked = 0;
					PickedColors();
					my_status = PICKLIST;
				}
			}

			if (my_status == SUFFIX && k == FL_Enter) {
				if (nbrPicked) {
					PickedToCallins (0);
					if (callin_is_up) {
						WhoIsUp = callinlist.numlist () - 1;
						dispCallIns(false);
					}
					my_status = LOGLIST;
				}
			}

			if ( (k >= 'A' && k <= 'z') || (k >= '0' && k <= '9') )	{
				if (isalpha (k))
					keyval[0] = toupper(k);
				else
					keyval[0] = k;
				switch (my_status) {
					case LOGLIST :
						szSuffix.clear();
						szSuffix = keyval;
						inpLoginSuffix->label (szSuffix.c_str());
						fillPickList ();
						my_status = SUFFIX;
						inpLoginSuffix->labelcolor (FL_WHITE);
						inpLoginSuffix->color (FL_DARK_BLUE);
						break;
					case SUFFIX :
						szSuffix.append(keyval);
						if (szSuffix.length() > 3) szSuffix.erase(0,1);
						inpLoginSuffix->label (szSuffix.c_str());
						fillPickList ();
						break;
					case PREFIX :
						szPrefix.append(keyval);
						if (szPrefix.length() > 2) szPrefix.erase(0,1);
						inpLoginPrefix->label (szPrefix.c_str());
						break;
					case AREA :
						szArea = keyval;
						inpLoginArea->label (szArea.c_str());
						break;
					default : ;
				}
				return 1;
			}
			if (k == FL_Tab) {
				switch (my_status) {
					case SUFFIX :
						inpLoginSuffix->labelcolor (FL_BLACK);
						inpLoginSuffix->color (FL_WHITE);
						inpLoginPrefix->labelcolor (FL_WHITE);
						inpLoginPrefix->color (FL_DARK_BLUE);
						inpLoginPrefix->label (szPrefix.c_str());
						my_status = PREFIX;
						break;
					case PREFIX :
						inpLoginPrefix->labelcolor (FL_BLACK);
						inpLoginPrefix->color (FL_WHITE);
						inpLoginArea->labelcolor (FL_WHITE);
						inpLoginArea->color (FL_DARK_BLUE);
						inpLoginArea->label (szArea.c_str());
						my_status = AREA;
						break;
					case AREA :
					default : 
						break;
				}
				return 1;
			}
			if (k == FL_Enter && my_status == AREA) {
				time_t the_time;
				struct tm *tm_ptr;
				char sztime[6];
				time (&the_time);
				tm_ptr = localtime (&the_time);
				sprintf( sztime, "%02d:%02d", tm_ptr->tm_hour, tm_ptr->tm_min);
				callinlist.add (-1, szPrefix.c_str(), szArea.c_str(), szSuffix.c_str(), "", sztime );
				if (disp_new_login) WhoIsUp = callinlist.numlist () - 1;
				dispCallIns (false);
				clearPickList ();
				clearSAP ();
				if (disp_new_login && open_editor) cb_F12 (WhoIsUp);
			}
		}
	}
	return 1;
}

